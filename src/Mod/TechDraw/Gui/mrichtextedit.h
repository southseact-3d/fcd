#ifndef MRICHTEXTEDIT_H_
#define MRICHTEXTEDIT_H_

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QPointer>

#include "ui_mrichtextedit.h"

/**
 * @brief A simple rich-text editor
 */
class MRichTextEdit : public QWidget, protected Ui::MRichTextEdit {
    Q_OBJECT

public:
    MRichTextEdit(QWidget *parent = nullptr, QString textIn = QString() );
    ~MRichTextEdit() override = default;

    QString toPlainText() const { return f_textedit->toPlainText(); }
    QString toHtml() const;
    QTextDocument *document() { return f_textedit->document(); }
    QTextCursor    textCursor() const { return f_textedit->textCursor(); }
    void           setTextCursor(const QTextCursor& cursor) { f_textedit->setTextCursor(cursor); }
    void setDefFontSize(int fontSize);
    void setDefFont(QString fontName);
    QString getDefFontSize();
    int getDefFontSizeNum();
    QFont getDefFont();

public Q_SLOTS:
    void setText(const QString &text);

Q_SIGNALS:
    void saveText(QString revText);
    void editorFinished();

protected:
  void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
  void fontChanged(const QFont &font);
  void fgColorChanged(const QColor &color);
  void bgColorChanged(const QColor &color);
  void list(bool checked, QTextListFormat::Style style);
  void indent(int delta);
  void focusInEvent(QFocusEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  bool hasMultipleSizes();

  void addFontSize(QString fontSize);

  enum ParagraphItems { ParagraphStandard = 0,
                        ParagraphHeading1,
                        ParagraphHeading2,
                        ParagraphHeading3,
                        ParagraphHeading4,
                        ParagraphMonospace };

protected Q_SLOTS:
    void onSave();
    void onExit();
    void setPlainText(const QString &text) { f_textedit->setPlainText(text); }
    void setHtml(const QString &text)      { f_textedit->setHtml(text); }
    void textRemoveFormat();
    void textRemoveAllFormat();
    void textBold();
    void textUnderline();
    void textStrikeout();
    void textItalic();
    void textSize(const QString &pointsAsString);
    void textLink(bool checked);
    void textStyle(int index);
    void textFgColor();
    void textBgColor();
    void listBullet(bool checked);
    void listOrdered(bool checked);
    void slotCurrentCharFormatChanged(const QTextCharFormat &format);
    void slotCursorPositionChanged();
    void slotClipboardDataChanged();
    void increaseIndentation();
    void decreaseIndentation();
    void insertImage();
    void textSource();
    void onSelectionChanged();

private:
    QStringList m_paragraphItems;
    int m_fontsize_h1;
    int m_fontsize_h2;
    int m_fontsize_h3;
    int m_fontsize_h4;

    QPointer<QTextList> m_lastBlockList;
    int m_defFontSize;
    QString m_defFont;

};

#endif
