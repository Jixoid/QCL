
#include <iostream>
#include <memory>
#include "Basis.h"
#include "qcl/Control.hh"
#include "qcl/Application.hh"
#include "qcl/View.hh"

// Mock control to track size during paint prepare
class TestControl : public qcl::control {
public:
    qcl::size_i32 SeenSize = {0, 0};
    
    void Do_Paint_prepare() override {
        SeenSize = Size;
        qcl::control::Do_Paint_prepare();
    }
};

extern "C" int qcl_entry2(std::shared_ptr<qcl::application> app) {
    std::cout << "Starting Resize Verification Test..." << std::endl;

    auto validView = std::make_shared<qcl::view>();
    validView->Size = {100, 100};

    auto child = std::make_shared<TestControl>();
    child->Size = {50, 50};
    // Anchor child to fill parent to ensure it resizes with parent
    child->Anchors.Left = {true, validView.get(), qcl::controlAnchorSide::casBegin};
    child->Anchors.Righ = {true, validView.get(), qcl::controlAnchorSide::casEnd};
    child->Anchors.Top  = {true, validView.get(), qcl::controlAnchorSide::casBegin};
    child->Anchors.Bot  = {true, validView.get(), qcl::controlAnchorSide::casEnd};
    
    validView->Child_Add(child);

    // Initial Tiling
    validView->Do_Paint_prepare();
    std::cout << "Initial Child Size: " << child->Size.W << "x" << child->Size.H << std::endl;

    // Resize Parent
    std::cout << "Resizing Parent to 200x200..." << std::endl;
    validView->Size = {200, 200};
    validView->Flag_Add(qcl::MustResize);

    // Run Paint Prepare
    // This should trigger Tiling (Top-Down) -> Child Update
    validView->Do_Paint_prepare();

    std::cout << "Child Size after Parent Prepare: " << child->Size.W << "x" << child->Size.H << std::endl;
    std::cout << "Child Seen Size in Prepare: " << child->SeenSize.W << "x" << child->SeenSize.H << std::endl;

    if (child->Size.W == 200 && child->Size.H == 200 && child->SeenSize.W == 200) {
        std::cout << "SUCCESS: Child resized synchronously." << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Child did not resize synchronously." << std::endl;
        return 1;
    }
}

extern "C" void qcl_error(const char *Msg)
{
  cerr << Msg << endl;
}

