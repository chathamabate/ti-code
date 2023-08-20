#pragma once 

#include "ti/screen.h"
#include <cxxutil/core/mem.h>
#include <cxxutil/gui/pane.h>
#include <cxxutil/gui/text_block.h>

#include <graphx.h>

namespace cxxutil { namespace gui {

    // Unlike like the scroll pane, all tree pane
    // text will have same dimmensions.
    struct text_color_info_t {
        uint8_t fgColor;
        uint8_t bgColor;

        inline void install() const {
            gfx_SetTextFGColor(this->fgColor);
            gfx_SetTextBGColor(this->bgColor);
        }

        inline void installInverse() const {
            gfx_SetTextFGColor(this->bgColor);
            gfx_SetTextBGColor(this->fgColor);
        }
    };

    // In the below classes type T MUST implement 
    // the following functions!
    //
    // const char *getLabel() const;
    // const text_color_info_t *getLabelInfo() const;
    // 
    template<typename T> class TreePaneNode;
    template<typename T> class TreePaneBranch;
    template<typename T> class TreePaneLeaf;
    
    // NOTE: a tree of TreePaneNode's is meant to have constant structure.
    // I make fields constants everywhere it is easy to do so.
    template<typename T>
    class TreePaneNode : public core::SafeObject {
        // See link() and setDepth() functions.
        friend class TreePaneBranch<T>;
        friend class TreePaneLeaf<T>;

    private: 
        // NOTE: These are backwards pointers.
        //
        // If this node has a parent, it will be stored in "parent",
        // ind refers to the index of this node's location in the 
        // parent's children array. (See TreePaneBranch)
        //
        // These will not be constants as they will be set after
        // the node is created.
        //
        // If this node has no parent, parent will be NULL, ind will be 0.
        TreePaneNode *parent;
        size_t ind;

        // This exists to aid with creating backwards pointers in
        // the tree. 
        //
        // This function will only be used in the constructor of the
        // TreePaneBranch. This is the only place where nodes should
        // be linked to their parents.
        void link(TreePaneNode *p, size_t i) {
            this->parent = p;
            this->ind = i;
        }

        // This is the relative depth of this node.
        // I really wanted to make this a constant an unexposed,
        // but it was just too hard to organize.
        //
        // This can be set at any time using the declareRoot() 
        // function.
        size_t depth;
        virtual void setDepth(size_t d) = 0;

        // Lastly, every node will hold an arbitrary state.
        // NOTE: JUST KNOW that when the state is given to this node
        // it becomes "part" of the node.
        // When this node is deleted, so is the state.
        T * const state;

    protected:
        TreePaneNode(uint8_t memChnl, T *s) 
            : core::SafeObject(memChnl), state(s)  {
            this->parent = nullptr;
            this->ind = 0;
            this->depth = 0;
        }

        TreePaneNode(T *s) : TreePaneNode(core::CXX_DEF_CHNL, s) {
        }

    public:
        virtual ~TreePaneNode() {
            delete this->state;
        }

        inline T *getState() {
            return this->state;
        }

        inline TreePaneNode *getParent() const {
            return this->parent;
        }

        inline size_t getIndex() const {
            return this->ind;
        }

        virtual bool isBranch() const = 0;
        inline bool isLeaf() const {
            return !(this->isBranch());
        }

        inline void declareRoot() {
            this->setDepth(0);
        }
        
        inline size_t getDepth() {
            return this->depth;
        }

        virtual void setExpanded(bool e) = 0;
        inline void expand() {
            this->setExpanded(true);
        }

        inline void minimize() {
            this->setExpanded(false);
        }

        inline void toggle() {
            this->setExpanded(!(this->isExpanded()));
        }

        virtual bool isExpanded() const = 0;
        inline bool isMinimized() const {
            return !(this->isExpanded());
        }

        virtual TreePaneNode * const *getChildren() const = 0;
        virtual size_t getChildrenLen() const = 0;

        // Get the number of nodes which are "reachable" from this node.
        // NOTE: this is not the traditional definition.
        //
        // Reachable(x) = 1 if isLeaf(x) or isMinimized(x)
        //                otherwise 1 + sum(reachable(c) forall children c)
        virtual size_t getNumReachable() const = 0;

        // Finds the next node above or below this node as it'd
        // be rendered. 
        //
        // Returns null if there are no where to go up or down.
        //
        // This assumes that this node is "reachable".
        // This means the path from the root of this node's tree
        // to this node only contains expanded nodes.
        // (excluding this node)
        TreePaneNode *nextUp() {
            if (this->ind == 0) {
                return this->parent;
            }

            // It is assumed that parent must be expanded.
            TreePaneNode *iter = 
                this->parent->getChildren()[this->ind - 1];

            while (!(iter->isLeaf() || iter->isMinimized())) {
                iter = iter->getChildren()[iter->getChildrenLen() - 1];
            }

            return iter;
        }

        TreePaneNode *nextDown() {
            if (this->isBranch() && this->isExpanded()) {
                return this->getChildren()[0];
            }

            TreePaneNode *iter = this;

            while (iter->parent) {
                // If the iterator is not the leftmost child.
                if (iter->ind < iter->parent->getChildrenLen() - 1) {
                    return iter->parent->getChildren()[iter->ind + 1];
                }

                iter = iter->parent;
            }

            // We made it all the way to the root!
            return nullptr;
        }
    };

    // NOTE: TreePaneBranch and TreePaneLeaf will remain abstract.
    // This way, the user can define how label text and styling is 
    // retrieved.

    template<typename T>
    class TreePaneBranch : public TreePaneNode<T> {
    private:
        virtual void setDepth(size_t d) override {
            this->depth = d;

            size_t len = this->children->getLen();
            for (size_t i = 0; i < len; i++) {
                this->children->get(i)->setDepth(d + 1);
            }
        }

        core::SafeArray<TreePaneNode<T> *> * const children;

        bool expanded;

    public:
        // NOTE: VERY IMPORTANT!!!!
        // 1) The chldn safe array will become "part" of this object. 
        // That is, when this branch node is deleted, all TreePaneNodes
        // in children will be deleted AND the array itself will be deleted.
        //
        // 2) The memory channel of this TreePaneBranch will be the memory
        // channel of the given children array.
        //
        // NOTE: The nodes in the given SafeArray must not belong to any other
        // tree. This will result in undefined behavoir.
        //
        // The design of these concepts is centered around these truths.
        //
        // NOTE: Undefined Behavoir if chldn is empty!
        TreePaneBranch(T *s, core::SafeArray<TreePaneNode<T> *> *chldn) 
            : TreePaneNode<T>(chldn->getChnl(), s), children(chldn) {
            size_t len = this->children->getLen();

            // NOTE: This is the ONLY place where we link parents!
            for (size_t i = 0; i < len; i++) {
                this->children->get(i)->link(this, i);
            }

            this->expanded = false;
        }

        virtual ~TreePaneBranch() {
            size_t len = this->children->getLen(); 
            for (size_t i = 0; i < len; i++) {
                delete this->children->get(i);
            }

            delete this->children;
        }

        virtual inline bool isBranch() const override {
            return true;
        }

        virtual inline void setExpanded(bool e) override {
            this->expanded = e;
        }

        virtual inline bool isExpanded() const override {
            return this->expanded;
        }

        virtual inline TreePaneNode<T> * const *getChildren() const override {
            return this->children->getArr();
        }

        virtual inline size_t getChildrenLen() const override {
            return this->children->getLen();
        }

        virtual size_t getNumReachable() const override {
            // NOTE: consider making this non-recursive to save
            // stack space.
            if (this->isMinimized()) {
                return 1;
            }
            
            size_t reachable = 1;      

            for (size_t i = 0; i < this->children->getLen(); i++) {
                reachable += this->children->get(i)->getNumReachable();
            }

            return reachable;
        }
    }; 

    template<typename T>
    class TreePaneLeaf : public TreePaneNode<T> {
    private:
        virtual void setDepth(size_t d) override {
            this->depth = d;
        }

    public:
        TreePaneLeaf(uint8_t memChnl, T *s) 
            : TreePaneNode<T>(memChnl, s) {
        }

        TreePaneLeaf(T *s) 
            : TreePaneLeaf<T>(core::CXX_DEF_CHNL, s) {
        }

        virtual ~TreePaneLeaf() {
        }

        virtual inline bool isBranch() const override {
            return false;
        }

        virtual inline void setExpanded(bool e) override {
            (void)e;
        }

        virtual inline bool isExpanded() const override {
            return false;
        }

        virtual inline TreePaneNode<T> * const *getChildren() const override {
            return nullptr;
        }

        virtual inline size_t getChildrenLen() const override {
            return 0;
        }

        virtual inline size_t getNumReachable() const override {
            return 1;
        }
    };

    // NOTE: TreePane will be used to display and modify a TreePaneNode.
    // Each TreePaneNode will take up a single row of the TreePane.
    //
    // A TreePaneNode's display style is given to the TreePane by the node.
    // * Like text block, the default 8x8 font will be used.
    //
    // Even if a TreePaneNode provides an empty string or null as its label,
    // its row will still be rendered with the provided height scale.
    //
    // The excess of a label will not be wrapped like in TextBlock. 
    // It'll just be cut off the right side of the TreePane.
    //
    // If a node is so deep in the tree that it's label is off screen,
    // the display will not shift at this time, space will simply be 
    // left for the node's row.
    // NOTE: Consider improving this at somepoint.

    struct tree_pane_info_t {
        // Pane width and height.
        uint8_t scrollBarWidth;
        uint8_t scrollBarFGColor;
        uint8_t scrollBarBGColor;

        uint24_t rowWidth;
        uint8_t height;

        uint8_t lblVertSpace; // Spacing between rows.

        // NOTE: 8 * height scale cannot be greater than the height
        // of the pane. In this case, UB.
        uint8_t lblWidthScale;
        uint8_t lblHeightScale;

        uint8_t paneBGColor;
        uint8_t selRowBGColor;
        uint8_t tabWidth;
    };

    template<typename T>
    class TreePane : public Pane {
    private: 
        const tree_pane_info_t * const paneInfo;

        // NOTE: The root will NOT be deleted when the pane is.
        TreePaneNode<T> * const root;

        // NOTE: this will be different from the TextBlock.
        // All rendering will occur relative to the selected line!
        TreePaneNode<T> *sel;

        // This will never be greater than height - (8*heightScale)
        uint8_t selRelY;

        // NOTE: these fields are used for rendering the scrollbar.
        
        // The 0-indexed row number of the current selection.
        size_t selRowInd;

        // The total number of rows in the whole pane. 
        // (Regardless of what is rendered)
        size_t totalRows;

        // x and y should be the top left corner of the node's row.
        // This will NOT install any text colors.
        void renderNode(uint24_t x, uint8_t y, TreePaneNode<T> *node) const {
            node->getState()->getLabelInfo()->install();

            uint24_t relX = node->getDepth() * this->paneInfo->tabWidth; 
            gfx_SetTextXY(x + relX, y);

            const char *lbl = node->getState()->getLabel();

            char currChar = *lbl;
            uint8_t currCharWidth = gfx_GetCharWidth(currChar);
            
            while (currChar != '\0' && relX + currCharWidth <= this->paneInfo->rowWidth) {
                gfx_PrintChar(currChar); 
                relX += currCharWidth;

                currChar = *(++lbl);
                currCharWidth = gfx_GetCharWidth(currChar);
            } 
        }

        // x and y are the top left corner of the entire pane.
        void renderScrollBar(uint24_t x, uint8_t y) const {
            const tree_pane_info_t * const pi = this->paneInfo;

            gfx_SetColor(pi->scrollBarBGColor);
            gfx_FillRectangle(x + pi->rowWidth, y, 
                    pi->scrollBarWidth, pi->height);

            if (!(this->isInFocus())) {
                return;
            }

            // Height of a row containing the following vertical space.
            const uint24_t fullRowHeight = pi->lblVertSpace + (pi->lblHeightScale * 8);

            uint24_t totalViewHeight = (this->selRowInd * fullRowHeight) - this->selRelY;

            uint24_t totalHeight = (this->totalRows * fullRowHeight) - pi->lblVertSpace;

            // In this case "total height" can be extended depending
            // on where the view is.
            if (totalViewHeight + pi->height > totalHeight) {
                totalHeight = totalViewHeight + pi->height;
            }

            // NOTE: it is impossible here for totalHeight to 
            // be less than pi->height.
            if (totalHeight == pi->height) {
                return;
            }

            uint8_t scrollBarRelY = (totalViewHeight * pi->height) / totalHeight;

            uint8_t scrollBarHeight = (totalHeight - totalViewHeight) > pi->height 
                ? (pi->height * pi->height) / totalHeight
                : pi->height - scrollBarRelY;
            
            gfx_SetColor(pi->scrollBarFGColor);

            gfx_FillRectangle(x + pi->rowWidth, y + scrollBarRelY, 
                    pi->scrollBarWidth, scrollBarHeight);
        }
    public:
        // UB when any given pointers are null.
        TreePane(uint8_t memChnl, const tree_pane_info_t *tpi, TreePaneNode<T> *r) 
            : Pane(memChnl), paneInfo(tpi), root(r) {
            this->sel = r;
            this->selRelY = 0;
            this->selRowInd = 0;
            this->totalRows = r->getNumReachable();
        }

        TreePane(const tree_pane_info_t *tpi, TreePaneNode<T> *r) 
            : TreePane(core::CXX_DEF_CHNL, tpi, r) {
        }

        virtual ~TreePane() {
        }

        virtual void render(uint24_t x, uint8_t y) const override {
            const tree_pane_info_t * const pi = this->paneInfo;

            gfx_SetColor(pi->paneBGColor);
            gfx_FillRectangle(x, y, pi->rowWidth, pi->height);

            gfx_SetTextScale(pi->lblWidthScale, pi->lblHeightScale);

            const uint8_t rowHeight = 8 * pi->lblHeightScale;

            // First render selected row.
            gfx_SetColor(pi->selRowBGColor);

            // TODO fix this shit...
            gfx_FillRectangle(x, y + this->selRelY - pi->lblVertSpace, pi->rowWidth, rowHeight + (2*pi->lblVertSpace));

            this->renderNode(x, y + this->selRelY, this->sel);

            // Next we render up to the top of the pane.
            // NOTE: it is gauranteed there exist lines
            // up to the top left corner of the pane.
            
            TreePaneNode<T> *iter = this->sel;
            uint8_t iterRelY = this->selRelY;    
            
            while (iterRelY >= pi->lblVertSpace + rowHeight) {
                iterRelY -= pi->lblVertSpace + rowHeight;

                iter = iter->nextUp();

                this->renderNode(x, y + iterRelY, iter);
            }
            
            iter = this->sel;
            iterRelY = this->selRelY;    

            while (pi->height - iterRelY >= pi->lblVertSpace + (2*rowHeight)) {
                iterRelY += rowHeight + pi->lblVertSpace;

                iter = iter->nextDown();

                if (!iter) {
                    break;
                }

                this->renderNode(x, y + iterRelY, iter);
            }

            this->renderScrollBar(x, y);
        }

        virtual void update(core::KeyManager *km) override {
        }

        virtual inline uint24_t getWidth() const override {
            return this->paneInfo->rowWidth + 
                this->paneInfo->scrollBarWidth;
        }

        virtual inline uint8_t getHeight() const override {
            return this->paneInfo->height;
        }

        // These three getters are primarily exposed to aid with testing.

        inline uint8_t getSelectedRowRelY() const {
            return this->selRelY;
        }

        inline size_t getSelectedRowInd() const {
            return this->selRowInd;
        }

        inline size_t getTotalRows() const {
            return this->totalRows;
        }

        // NOTE: these below functions help traversing tree based on how it
        // it is expanded.
        
        inline void gotoTop() {
            this->sel = this->root;
            this->selRelY = 0;
        }

        // Move the selected line up and down.
        void scrollDown() {
            TreePaneNode<T> *next = this->sel->nextDown();

            if (!next) {
                return; // No where to go!
            }

            this->sel = next;
            this->selRowInd++;

            const uint8_t rowHeight = 8 * this->paneInfo->lblHeightScale;
            const tree_pane_info_t * const pi = this->paneInfo;

            // Not enough pane space.
            if (pi->height - this->selRelY < pi->lblVertSpace + (2*rowHeight)) {
                this->selRelY = pi->height - rowHeight;
            } else {
                this->selRelY += pi->lblVertSpace + rowHeight;
            }
        }

        void scrollUp() {
            TreePaneNode<T> *next = this->sel->nextUp();

            if (!next) {
                return; // No where to go!
            }

            this->sel = next;
            this->selRowInd--;

            const uint8_t rowHeight = 8 * this->paneInfo->lblHeightScale;
            const tree_pane_info_t * const pi = this->paneInfo;

            if (this->selRelY < pi->lblVertSpace + rowHeight) {
                this->selRelY = 0;
            } else {
                this->selRelY -= pi->lblVertSpace + rowHeight;
            }
        }

        // Toggle expand/minimize of the selected node.
        void toggle() {
            if (this->sel->isExpanded()) {
                size_t reachableChildren = this->sel->getNumReachable() - 1;
                this->totalRows -= reachableChildren;

                this->sel->minimize();
            } else {
                this->sel->expand();

                size_t reachableChildren = this->sel->getNumReachable() - 1;
                this->totalRows += reachableChildren;
            }
        }
    }; 
}}
