/* *******************************************************************************
 *                                                                               *
 *  Copyright 2026 Trollycat                                                     *
 *                                                                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");              *
 *  you may not use this file except in compliance with the License.             *
 *  You may obtain a copy of the License at                                      *
 *                                                                               *
 *      http://www.apache.org/licenses/LICENSE-2.0                               *
 *                                                                               *
 *  Unless required by applicable law or agreed to in writing, software          *
 *  distributed under the License is distributed on an "AS IS" BASIS,            *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     *
 *  See the License for the specific language governing permissions and          *
 *  limitations under the License.                                               *
 *                                                                               *
 *********************************************************************************
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Virtual address descriptor                                         *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Binary search tree kept by the kernel to manage fake vmem          *
 ********************************************************************************/
#include <cbk/mm/vad.h>

#define NODE_HEIGHT(n) ((n) ? (n)->height : 0)
#define MAX_HEIGHT(a, b) ((a) > (b) ? (a) : (b))

namespace cbk::mem
{
    namespace
    {
        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : VadUpdateHeight                                                     *
         * DATE    : 2026                                                                *
         * PURPOSE : Re-calculates and caches the max depth of a node                    *
         ********************************************************************************/
        INLINE VOID
        VadUpdateHeight(PMMVAD node) noexcept
        {
            if (node != nullptr)
                node->height =
                    MAX_HEIGHT(NODE_HEIGHT(node->left_child), NODE_HEIGHT(node->right_child)) + 1;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : VadGetBalance                                                       *
         * DATE    : 2026                                                                *
         * PURPOSE : Computes balance difference (left height - right height)            *
         ********************************************************************************/
        NO_DISCARD INLINE LONG
        VadGetBalance(PMMVAD node) noexcept
        {
            return node ? (NODE_HEIGHT(node->left_child) - NODE_HEIGHT(node->right_child)) : 0;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : VadRotateLeft                                                       *
         * DATE    : 2026                                                                *
         * PURPOSE : Single left rotation for right-heavy tree                           *
         ********************************************************************************/
        NO_DISCARD INLINE PMMVAD
        VadRotateLeft(PMMVAD x) noexcept
        {
            PMMVAD y       = x->right_child;
            x->right_child = y->left_child;

            if (y->left_child != nullptr)
                y->left_child->parent = x;

            y->parent     = x->parent;
            y->left_child = x;
            x->parent     = y;

            VadUpdateHeight(x);
            VadUpdateHeight(y);
            return y;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : VadRotateRight                                                      *
         * DATE    : 2026                                                                *
         * PURPOSE : Single right rotation for left-heavy tree                           *
         ********************************************************************************/
        NO_DISCARD INLINE PMMVAD
        VadRotateRight(PMMVAD y) noexcept
        {
            PMMVAD x      = y->left_child;
            y->left_child = x->right_child;

            if (x->right_child != nullptr)
                x->right_child->parent = y;

            x->parent      = y->parent;
            x->right_child = y;
            y->parent      = x;

            VadUpdateHeight(y);
            VadUpdateHeight(x);
            return x;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : VadRebalanceTree                                                    *
         * DATE    : 2026                                                                *
         * PURPOSE : Upward chains to correct AVL balance                                *
         ********************************************************************************/
        VOID
        VadRebalanceTree(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept
        {
            while (node != nullptr) {
                VadUpdateHeight(node);
                LONG balance = VadGetBalance(node);

                if (balance > 1) {
                    if (VadGetBalance(node->left_child) < 0)
                        node->left_child = VadRotateLeft(node->left_child);

                    PMMVAD parent       = node->parent;
                    PMMVAD balanced_sub = VadRotateRight(node);

                    if (parent == nullptr)
                        space->vad_root = balanced_sub;
                    else if (parent->left_child == node)
                        parent->left_child = balanced_sub;
                    else
                        parent->right_child = balanced_sub;

                    node = parent;
                    continue;
                }

                if (balance < -1) {
                    if (VadGetBalance(node->right_child) > 0)
                        node->right_child = VadRotateRight(node->right_child);

                    PMMVAD parent       = node->parent;
                    PMMVAD balanced_sub = VadRotateLeft(node);

                    if (parent == nullptr)
                        space->vad_root = balanced_sub;
                    else if (parent->left_child == node)
                        parent->left_child = balanced_sub;
                    else
                        parent->right_child = balanced_sub;

                    node = parent;
                    continue;
                }

                node = node->parent;
            }
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : VadFindMinimum                                                     *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Finds the VAD node with the lowest address in a sub-tree           *
         ********************************************************************************/
        NO_DISCARD PMMVAD
        VadFindMinimum(PMMVAD node) noexcept
        {
            if (node == nullptr)
                return nullptr;

            while (node->left_child != nullptr)
                node = node->left_child;

            return node;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : VadGetNextSuccessor                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Finds the next node in memory order                                *
         ********************************************************************************/
        NO_DISCARD PMMVAD
        VadGetNextSuccessor(PMMVAD node) noexcept
        {
            if (node == nullptr)
                return nullptr;
            if (node->right_child != nullptr)
                return VadFindMinimum(node->right_child);

            PMMVAD p = node->parent;

            while (p != nullptr && node == p->right_child) {
                node = p;
                p    = p->parent;
            }

            return p;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : VadLinkChildNode                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Link a child leaf to its parent                                    *
         ********************************************************************************/
        VOID
        VadLinkChildNode(PMMVAD parent, PMMVAD *child_slot, PMMVAD new_node) noexcept
        {
            *child_slot           = new_node;
            new_node->parent      = parent;
            new_node->left_child  = nullptr;
            new_node->right_child = nullptr;
        }

    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CreateVadNode                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Creates a new VAD node                                             *
     ********************************************************************************/
    NO_DISCARD PMMVAD
    VadInitializeNode(PMMVAD blank_node,
                      QWORD starting_vpn,
                      SIZE_T page_count,
                      ULONG protect) noexcept
    {
        if (blank_node == nullptr)
            return nullptr;

        blank_node->starting_vpn   = starting_vpn;
        blank_node->ending_vpn     = starting_vpn + page_count - 1;
        blank_node->left_child     = nullptr;
        blank_node->right_child    = nullptr;
        blank_node->parent         = nullptr;
        blank_node->balance        = 0;
        blank_node->backing_object = nullptr;

        blank_node->u.long_flags       = 0;
        blank_node->u.flags.protection = protect & 0x1F;
        blank_node->u.flags.vad_type   = VAD_PRIVATE;
        blank_node->u.flags.is_private = TRUE;

        return blank_node;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadFindNode                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree to see if a vpn exists inside a range               *
     ********************************************************************************/
    NO_DISCARD PMMVAD
    VadFindNode(PMM_ADDRESS_SPACE space, QWORD vpn) noexcept
    {
        if (space == nullptr)
            return nullptr;

        PMMVAD current = space->vad_root;

        while (current != nullptr) {

            if (vpn >= current->starting_vpn && vpn <= current->ending_vpn)
                return current;

            if (vpn < current->starting_vpn)
                current = current->left_child;
            else
                current = current->right_child;
        }

        return nullptr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadInsertNode                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Places a new block into the binary tree                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    VadInsertNode(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept
    {
        if (space == nullptr || node == nullptr)
            return STATUS_INVALID_PARAMETER;

        if (space->vad_root == nullptr) {
            VadLinkChildNode(nullptr, &space->vad_root, node);
            return STATUS_SUCCESS;
        }

        PMMVAD current = space->vad_root;
        while (true) {

            if (node->ending_vpn < current->starting_vpn) {

                if (current->left_child == nullptr) {
                    VadLinkChildNode(current, &current->left_child, node);
                    break;
                }

                current = current->left_child;

            } else if (node->starting_vpn > current->ending_vpn) {

                if (current->right_child == nullptr) {
                    VadLinkChildNode(current, &current->right_child, node);
                    break;
                }

                current = current->right_child;

            } else
                return STATUS_CONFLICTING_ADDRESSES;
        }

        VadRebalanceTree(space, node->parent);
        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : VadDeleteNode                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Removes a node from the  tree and rebalances                        *
     ********************************************************************************/
    VOID
    VadDeleteNode(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept
    {
        if (space == nullptr || node == nullptr)
            return;

        PMMVAD rebalance_start = nullptr;

        if (node->left_child == nullptr || node->right_child == nullptr) {
            PMMVAD child    = (node->left_child != nullptr) ? node->left_child : node->right_child;
            rebalance_start = node->parent;

            if (child != nullptr)
                child->parent = node->parent;

            if (node->parent == nullptr)
                space->vad_root = child;
            else if (node->parent->left_child == node)
                node->parent->left_child = child;
            else
                node->parent->right_child = child;
        } else {
            PMMVAD successor = VadFindMinimum(node->right_child);
            rebalance_start  = successor->parent;

            if (successor->parent->left_child == successor)
                successor->parent->left_child = successor->right_child;
            else
                successor->parent->right_child = successor->right_child;

            if (successor->right_child != nullptr)
                successor->right_child->parent = successor->parent;

            if (rebalance_start == node)
                rebalance_start = successor;

            successor->parent      = node->parent;
            successor->left_child  = node->left_child;
            successor->right_child = node->right_child;
            successor->height      = node->height;

            if (node->left_child != nullptr)
                node->left_child->parent = successor;
            if (node->right_child != nullptr)
                node->right_child->parent = successor;

            if (node->parent == nullptr)
                space->vad_root = successor;
            else if (node->parent->left_child == node)
                node->parent->left_child = successor;
            else
                node->parent->right_child = successor;
        }

        if (rebalance_start != nullptr)
            VadRebalanceTree(space, rebalance_start);

        node->left_child  = nullptr;
        node->right_child = nullptr;
        node->parent      = nullptr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadFindFreeGap                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree looking for an empty hole between nodes             *
     ********************************************************************************/
    NO_DISCARD QWORD
    VadFindFreeGap(PMM_ADDRESS_SPACE space, SIZE_T page_cnt, BOOL top_down) noexcept
    {
        if (space == nullptr || space->vad_root == nullptr)
            return space->lowest_addr >> 12;

        QWORD current_vpn_frontier = space->lowest_addr >> 12;
        PMMVAD current             = VadFindMinimum(space->vad_root);

        while (current != nullptr) {

            if (current->starting_vpn > current_vpn_frontier) {
                SIZE_T gap_size = current->starting_vpn - current_vpn_frontier;
                if (gap_size >= page_cnt)
                    return current_vpn_frontier;
            }

            if (current->ending_vpn == 0xFFFFFFFFFFFFFFFF)
                return 0;

            current_vpn_frontier = current->ending_vpn + 1;
            current              = VadGetNextSuccessor(current);
        }

        QWORD highest_vpn_boundary = space->highest_addr >> 12;
        if (highest_vpn_boundary > current_vpn_frontier)
            if ((highest_vpn_boundary - current_vpn_frontier) >= page_cnt)
                return current_vpn_frontier;

        return 0;
    }

} // namespace cbk::mem