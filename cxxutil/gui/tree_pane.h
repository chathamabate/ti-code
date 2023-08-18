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
    
    // NOTE: a tree of TreePaneNode's is meant to have constant structure.
    // I make fields constants everywhere it is easy to do so.
    class TreePaneNode : public core::SafeObject {
        // See link() and setDepth() functions.
        friend class TreePaneBranch;
        friend class TreePaneLeaf;

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
        void link(TreePaneNode *p, size_t i);

        // This is the relative depth of this node.
        // I really wanted to make this a constant an unexposed,
        // but it was just too hard to organize.
        //
        // This can be set at any time using the declareRoot() 
        // function.
        size_t depth;
        virtual void setDepth(size_t d) = 0;

    protected:
        TreePaneNode(uint8_t memChnl);
        TreePaneNode();

    public:
        virtual ~TreePaneNode();

        inline TreePaneNode *getParent() const {
            return this->parent;
        }

        inline size_t getIndex() const {
            return this->ind;
        }

        virtual const text_color_info_t *getTreeLabelInfo() const = 0;

        inline void install() const {
            this->getTreeLabelInfo()->install();
        }

        inline void installInverse() const {
            this->getTreeLabelInfo()->installInverse();
        }

        virtual const char *getLabel() const = 0;

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

        // Finds the next node above or below this node as it'd
        // be rendered. 
        //
        // Returns null if there are no where to go up or down.
        //
        // This assumes that this node is "reachable".
        // This means the path from the root of this node's tree
        // to this node only contains expanded nodes.
        // (excluding this node)
        TreePaneNode *nextUp(); 
        TreePaneNode *nextDown(); 
    };

    // NOTE: TreePaneBranch and TreePaneLeaf will remain abstract.
    // This way, the user can define how label text and styling is 
    // retrieved.

    class TreePaneBranch : public TreePaneNode {
    private:
        virtual void setDepth(size_t d) override;

        core::SafeArray<TreePaneNode *> * const children;

        bool expanded;
    protected:
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
        TreePaneBranch(core::SafeArray<TreePaneNode *> *chldn);
    public:
        virtual ~TreePaneBranch();

        virtual inline bool isBranch() const override {
            return true;
        }

        virtual inline void setExpanded(bool e) override {
            this->expanded = e;
        }

        virtual inline bool isExpanded() const override {
            return this->expanded;
        }

        virtual inline TreePaneNode * const *getChildren() const override {
            return this->children->getArr();
        }

        virtual inline size_t getChildrenLen() const override {
            return this->children->getLen();
        }
    }; 

    class TreePaneLeaf : public TreePaneNode {
    protected:
        virtual void setDepth(size_t d) override;

        TreePaneLeaf(uint8_t memChnl);
        TreePaneLeaf();

    public:
        virtual ~TreePaneLeaf();

        virtual inline bool isBranch() const override {
            return false;
        }

        virtual inline void setExpanded(bool e) override {
            (void)e;
        }

        virtual inline bool isExpanded() const override {
            return false;
        }

        virtual inline TreePaneNode * const *getChildren() const override {
            return nullptr;
        }

        virtual inline size_t getChildrenLen() const override {
            return 0;
        }
    };

    // NOTE: TreePane will be used to display and modify a TreePaneNode.
    // Each TreePaneNode will take up a single row of the TreePane.
    //
    // A TreePaneNode's display style is given to the TreePane by the node.
    // * Like text block, the default 8x8 font will be used.
    // * If the height of a Node's scaled text would be larger than the TreePane
    // itself, the text scale used will be 1.
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
        uint24_t width;
        uint8_t height;

        uint8_t paneBGColor;
        uint8_t tabWidth;
    };

    // While TreeNode will remain abstract, TreePane will be concrete.
    
    class TreePane : Pane {
    private: 
        const tree_pane_info_t * const paneInfo;

        // NOTE: The root will NOT be deleted when the pane is.
        TreePaneNode * const root;

        // NOTE: this will be different from the TextBlock.
        // All rendering will occur relative to the selected line!
        TreePaneNode *sel;
        uint8_t selRelY;
        
    public:
        // UB when any given pointers are null.
        TreePane(uint8_t memChnl, const tree_pane_info_t *tpi, TreePaneNode *r); 
        TreePane(const tree_pane_info_t *tpi, TreePaneNode *r);
        virtual ~TreePane();

        virtual void render(uint24_t x, uint8_t y) const override;

        virtual void update(core::KeyManager *km) override;

        virtual inline uint24_t getWidth() const override {
            return this->paneInfo->width;
        }

        virtual inline uint8_t getHeight() const override {
            return this->paneInfo->height;
        }

        // NOTE: these below functions help traversing tree based on how it
        // is expanded.

        // Move the selected line up and down.
        void scrollDown();
        void scrollUp();
    }; 
}}
