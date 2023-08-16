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

        // NOTE: We cannot change the pointers themselves in the array.
        virtual const core::SafeArray<TreePaneNode *> *getChildren() const = 0; 
    };


    class TreePaneBranch : public TreePaneNode {
        // Maybe figure this out big time....
    }; 

    class TreePaneLeaf : public TreePaneNode {
        // What should really go in here..
    };
}}
