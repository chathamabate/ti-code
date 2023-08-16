#pragma once 

#include <cxxutil/core/mem.h>

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
    };
    
    // These nodes are meant to reside in dynamic memory.
    class TreePaneNode : public core::SafeObject {
    public:
        TreePaneNode(uint8_t memChnl);
        TreePaneNode();

        virtual ~TreePaneNode();

        virtual tree_label_info_t getTreeLabelInfo() const = 0;
        inline void installTreeLabelInfo() const {
            this->getTreeLabelInfo().install();
        }

        virtual const char *getLabel() const = 0;

        virtual bool isBranch() const = 0;
        inline bool isLeaf() const {
            return !(this->isBranch());
        }

        virtual TreePaneNode * const *getChildren() const = 0;
        virtual size_t getChildrenLen() const = 0;
    };


    class TreePaneBranch : public TreePaneNode {
    private:
        core::SafeArray<TreePaneNode *> * const children;
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

        virtual inline TreePaneNode * const *getChildren() const override {
            return nullptr;
        }

        virtual inline size_t getChildrenLen() const override {
            return 0;
        }
    };

    class TreePane : Pane {

    };
}}
