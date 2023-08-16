#pragma once 

#include <cxxutil/core/mem.h>
#include <cxxutil/gui/pane.h>

#include <graphx.h>

namespace cxxutil { namespace gui {
    
    // NOTE: for now, label info will only contain color.
    // Unlike text block, all labels will use the same sized font!
    struct tree_label_info_t {
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
    
    // These nodes are meant to reside in dynamic memory.
    class TreePaneNode : public core::SafeObject {
    public:
        TreePaneNode(uint8_t memChnl);
        TreePaneNode();

        virtual ~TreePaneNode();

        virtual tree_label_info_t getTreeLabelInfo() const = 0;

        inline void install() const {
            this->getTreeLabelInfo().install();
        }

        inline void installInverse() const {
            this->getTreeLabelInfo().installInverse();
        }

        virtual const char *getLabel() const = 0;

        virtual bool isBranch() const = 0;
        inline bool isLeaf() const {
            return !(this->isBranch());
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
    };

    // NOTE: TreePaneBranch and TreePaneLeaf will remain abstract.
    // This way, the user can define how label text and styling is 
    // retrieved.

    class TreePaneBranch : public TreePaneNode {
    private:
        core::SafeArray<TreePaneNode *> * const children;

        bool expanded;
    public:

        // NOTE: VERY IMPORTANT!!!!
        // 1) The chldn safe array will become "part" of this object. 
        // That is, when this branch node is deleted, all TreePaneNodes
        // in children will be deleted AND the array itself will be deleted.
        //
        // 2) The memory channel of this TreePaneBranch will be the memory
        // channel of the given children array.
        TreePaneBranch(core::SafeArray<TreePaneNode *> *chldn);
        ~TreePaneBranch();

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
    public:
        TreePaneLeaf(uint8_t memChnl);
        TreePaneLeaf();

        ~TreePaneLeaf();

        virtual inline bool isBranch() const override {
            return false;
        }

        virtual inline void setExpanded(bool e) override {
            // Do nothing.
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

    struct tree_pane_info_t {
        // Pane width and height.
        uint24_t width;
        uint8_t height;

        // label info for the tree pane.
        // NOTE: all labels will have the same font. 
        uint8_t lblWidthScale; 
        uint8_t lblHeightScale;
        uint8_t lblMonospace;

        uint8_t paneBGColor;

        uint8_t tabWidth;
    };

    // While TreeNode will remain abstract, TreePane will be concrete.
    
    class TreePane : Pane {
    private: 
        const tree_pane_info_t * const paneInfo;

        // NOTE: VERY IMPORTANT 
        // Just like the children array of the TreePaneBranch,
        // root will be a "part" of the tree pane.
        //
        // The root must be built first before given to the pane.
        // 
        // BUT, when the TreePane is deleted, the root will be
        // deleted too! 
        TreePaneNode * const root;

    public:
        // UB when any given pointers are null.
        TreePane(uint8_t memChnl, const tree_pane_info_t *tpi, TreePaneNode *r); 
        TreePane(const tree_pane_info_t *tpi, TreePaneNode *r);
        ~TreePane();

        virtual void render(uint24_t x, uint8_t y) const override;
        virtual void update(core::KeyManager *km) override;

        virtual uint24_t getWidth() const override;
        virtual uint8_t getHeight() const override; 
    };
}}
