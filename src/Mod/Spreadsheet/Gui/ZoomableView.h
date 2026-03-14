#ifndef ZOOMABLEVIEW_H
#define ZOOMABLEVIEW_H

#include <QGraphicsView>

#include "SpreadsheetView.h"

namespace SpreadsheetGui
{
class SheetTableView;
}

class ZoomableView: public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(int zoomLevel READ zoomLevel() WRITE setZoomLevel NOTIFY zoomLevelChanged)
public:
    /*!
     * \brief A descendant of QGraphicsView to show a SheetTableView object in its viewport,
     * allowing magnification. \param ui \details The object replaces SheetTableView in layout,
     * handling mouse and keyboard events.
     */
    explicit ZoomableView(Ui::Sheet* ui);
    ~ZoomableView() override = default;

    int zoomLevel() const;
    void setZoomLevel(int new_scale);


    static constexpr int min {60}, max {160};
    static void checkLimits(int& zoom_level);

Q_SIGNALS:
    void zoomLevelChanged(int);  /// This signal is emitted whenever zoom level is changed. It is
                                 /// used to show the zoom level in the zoom button.

public Q_SLOTS:
    void zoomIn(void);   /// This function is the slot for the zoomIn button and a keyboard shortcut
    void zoomOut(void);  /// This function is the slot for the zoomOut button and a keyboard shortcut
    void resetZoom(void);  /// This function is the slot for a keyboard shortcut

private:
    void updateView(void);

    QPointer<SpreadsheetGui::SheetTableView> stv;
    QGraphicsScene m_scene;
    QGraphicsProxyWidget* qpw {nullptr};

    int m_zoomLevel {0};

protected:
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    static constexpr int zoom_step_mwheel {5}, zoom_step_kb {10};
};

#endif  // ZOOMABLEVIEW_H
