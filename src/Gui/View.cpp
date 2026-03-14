#include "View.h"
#include "Application.h"
#include "Document.h"

using namespace Gui;

//**************************************************************************
// BaseView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE_ABSTRACT(Gui::BaseView, Base::BaseClass)


BaseView::BaseView(Gui::Document* pcDocument)
    : _pcDocument(pcDocument)
{
    if (pcDocument) {
        pcDocument->attachView(this);
        bIsPassive = false;
    }
    else {
        Application::Instance->attachView(this);
        bIsPassive = true;
    }
}

BaseView::~BaseView()
{
    onClose();
}

void BaseView::onClose()
{
    if (bIsDetached) {
        return;
    }

    if (bIsPassive) {
        Application::Instance->detachView(this);
        if (_pcDocument) {
            _pcDocument->detachView(this, true);
        }
    }
    else {
        if (_pcDocument) {
            _pcDocument->detachView(this);
        }
    }

    _pcDocument = nullptr;
    bIsDetached = true;
}

void BaseView::deleteSelf()
{
    delete this;
}

void BaseView::setDocument(Gui::Document* pcDocument)
{
    if (_pcDocument == pcDocument) {
        return;
    }
    // detaches and attaches the observer
    if (_pcDocument) {
        _pcDocument->detachView(this, true);
    }
    if (pcDocument) {
        pcDocument->attachView(this, true);
    }

    // set the new document as the active one
    _pcDocument = pcDocument;
}

/// returns the document the view is attached to
App::Document* BaseView::getAppDocument() const
{
    return _pcDocument ? _pcDocument->getDocument() : nullptr;
}
