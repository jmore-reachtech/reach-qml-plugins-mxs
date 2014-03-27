/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONTEXT2D_H
#define CONTEXT2D_H

#include <QPainter>
#include <QPainterPath>
#include <QString>
#include <QStack>
#include <QMetaType>
#include <QTimerEvent>
#include <QImage>
#include <QColor>
#include <QVariantList>
#include <QTime>
#include <QTimer>

#include <qdebug.h>

#include <QPixmap>

QColor colorFromString(const QString &name);

class CanvasGradient : public QObject
{
    Q_OBJECT
public:
    CanvasGradient(const QGradient &gradient) : m_gradient(gradient) {}

public slots:
    QGradient value() { return m_gradient; }
    void addColorStop(float pos, const QString &color) { m_gradient.setColorAt(pos, colorFromString(color));}

public:
    QGradient m_gradient;
};

Q_DECLARE_METATYPE(CanvasGradient*)


class CanvasImage: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString src READ src WRITE setSrc NOTIFY sourceChanged)
    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)

public:
    CanvasImage() {}
    CanvasImage(const QString &url) : m_image(url), m_src(url) {}
    CanvasImage(const QPixmap &pixmap) {m_image = pixmap;}

public slots:
    int width() { return m_image.width(); }
    int height() { return m_image.height(); }
    QPixmap &value() { return m_image; }
    QString src() { return m_src; }
    void setSrc(const QString &src) { m_src = src; m_image.load(src); emit sourceChanged();}
signals:
    void sourceChanged();

private:
    QPixmap m_image;
    QString m_src;
};

Q_DECLARE_METATYPE(CanvasImage*)


class ImageData {
};


class Context2D : public QObject
    {
    Q_OBJECT
    // compositing
    Q_PROPERTY(qreal globalAlpha READ globalAlpha WRITE setGlobalAlpha)
    Q_PROPERTY(QString globalCompositeOperation READ globalCompositeOperation WRITE setGlobalCompositeOperation)
    Q_PROPERTY(QVariant strokeStyle READ strokeStyle WRITE setStrokeStyle)
    Q_PROPERTY(QVariant fillStyle READ fillStyle WRITE setFillStyle)
    // line caps/joins
    Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(QString lineCap READ lineCap WRITE setLineCap)
    Q_PROPERTY(QString lineJoin READ lineJoin WRITE setLineJoin)
    Q_PROPERTY(qreal miterLimit READ miterLimit WRITE setMiterLimit)
    // shadows
    Q_PROPERTY(qreal shadowOffsetX READ shadowOffsetX WRITE setShadowOffsetX)
    Q_PROPERTY(qreal shadowOffsetY READ shadowOffsetY WRITE setShadowOffsetY)
    Q_PROPERTY(qreal shadowBlur READ shadowBlur WRITE setShadowBlur)
    Q_PROPERTY(QString shadowColor READ shadowColor WRITE setShadowColor)
    // fonts
    Q_PROPERTY(QString font READ font WRITE setFont)
    Q_PROPERTY(QString textBaseline READ textBaseline WRITE setTextBaseline)
    Q_PROPERTY(QString textAlign READ textAlign WRITE setTextAlign)

    enum TextBaseLine { Alphabetic=0, Top, Middle, Bottom, Hanging};
    enum TextAlign { Start=0, End, Left, Right, Center};

public:
    Context2D(QObject *parent = 0);
    void setSize(int width, int height);
    void setSize(const QSize &size);
    QSize size() const;

    void scheduleChange();
    void timerEvent(QTimerEvent *e);

    void clear();
    void reset();

    QPixmap pixmap() { return m_pixmap; }

    // compositing
    qreal globalAlpha() const; // (default 1.0)
    QString globalCompositeOperation() const; // (default over)
    QVariant strokeStyle() const; // (default black)
    QVariant fillStyle() const; // (default black)

    void setGlobalAlpha(qreal alpha);
    void setGlobalCompositeOperation(const QString &op);
    void setStrokeStyle(const QVariant &style);
    void setFillStyle(const QVariant &style);

    // line caps/joins
    qreal lineWidth() const; // (default 1)
    QString lineCap() const; // "butt", "round", "square" (default "butt")
    QString lineJoin() const; // "round", "bevel", "miter" (default "miter")
    qreal miterLimit() const; // (default 10)

    void setLineWidth(qreal w);
    void setLineCap(const QString &s);
    void setLineJoin(const QString &s);
    void setMiterLimit(qreal m);

    void setFont(const QString &font);
    QString font();
    void setTextBaseline(const QString &font);
    QString textBaseline();
    void setTextAlign(const QString &font);
    QString textAlign();

    // shadows
    qreal shadowOffsetX() const; // (default 0)
    qreal shadowOffsetY() const; // (default 0)
    qreal shadowBlur() const; // (default 0)
    QString shadowColor() const; // (default black)

    void setShadowOffsetX(qreal x);
    void setShadowOffsetY(qreal y);
    void setShadowBlur(qreal b);
    void setShadowColor(const QString &str);

public slots:
    void save(); // push state on state stack
    void restore(); // pop state stack and restore state

    void fillText(const QString &text, qreal x, qreal y);
    void strokeText(const QString &text, qreal x, qreal y);

    void setInPaint(bool val){m_inPaint = val;}
    void scale(qreal x, qreal y);
    void rotate(qreal angle);
    void translate(qreal x, qreal y);
    void transform(qreal m11, qreal m12, qreal m21, qreal m22,
                   qreal dx, qreal dy);
    void setTransform(qreal m11, qreal m12, qreal m21, qreal m22,
                      qreal dx, qreal dy);

    CanvasGradient *createLinearGradient(qreal x0, qreal y0,
                                         qreal x1, qreal y1);
    CanvasGradient *createRadialGradient(qreal x0, qreal y0,
                                         qreal r0, qreal x1,
                                         qreal y1, qreal r1);

    // rects
    void clearRect(qreal x, qreal y, qreal w, qreal h);
    void fillRect(qreal x, qreal y, qreal w, qreal h);
    void strokeRect(qreal x, qreal y, qreal w, qreal h);

    // path API
    void beginPath();
    void closePath();
    void moveTo(qreal x, qreal y);
    void lineTo(qreal x, qreal y);
    void quadraticCurveTo(qreal cpx, qreal cpy, qreal x, qreal y);
    void bezierCurveTo(qreal cp1x, qreal cp1y,
                       qreal cp2x, qreal cp2y, qreal x, qreal y);
    void arcTo(qreal x1, qreal y1, qreal x2, qreal y2, qreal radius);
    void rect(qreal x, qreal y, qreal w, qreal h);
    void arc(qreal x, qreal y, qreal radius,
             qreal startAngle, qreal endAngle,
             bool anticlockwise);
    void fill();
    void stroke();
    void clip();
    bool isPointInPath(qreal x, qreal y) const;

    CanvasImage *createImage(const QString &url);

    int measureText(const QString &text);

    // drawing images (no overloads due to QTBUG-11604)
    void drawImage(const QVariant &var, qreal dx, qreal dy, qreal dw, qreal dh);

    void moveCanvasImage(qreal sx, qreal sy, qreal sw, qreal sh, qreal dx, qreal dy);

    void drawWaveFormLine(qreal sx, qreal sy, qreal sw, qreal sh, qreal dx, qreal dy,
                                        QString bgColor, qreal x, qreal y, qreal w, qreal h,
                                        QString lineColor, qreal lineWidth, qreal x1, qreal y1, qreal x2, qreal y2,
                                        bool drawBlock);

    void drawWaveFormLine(qreal sx, qreal sy, qreal sw, qreal sh, qreal dx, qreal dy,
                                        QString bgColor, qreal x, qreal y, qreal w, qreal h, qreal lineWidth,
                                        QString lineColor1, qreal x11, qreal y11, qreal x12, qreal y12,
                                        QString lineColor2, qreal x21, qreal y21, qreal x22, qreal y22,
                                        bool drawBlock);
    void drawWaveFormLine(qreal sx, qreal sy, qreal sw, qreal sh, qreal dx, qreal dy,
                                        QString bgColor, qreal x, qreal y, qreal w, qreal h, qreal lineWidth,
                                        QString lineColor1, qreal x11, qreal y11, qreal x12, qreal y12,
                                        QString lineColor2, qreal x21, qreal y21, qreal x22, qreal y22,
                                        QString lineColor3, qreal x31, qreal y31, qreal x32, qreal y32,
                                        bool drawBlock);
    void drawWaveFormLine(qreal sx, qreal sy, qreal sw, qreal sh, qreal dx, qreal dy,
                                        QString bgColor, qreal x, qreal y, qreal w, qreal h, qreal lineWidth,
                                        QString lineColor1, qreal x11, qreal y11, qreal x12, qreal y12,
                                        QString lineColor2, qreal x21, qreal y21, qreal x22, qreal y22,
                                        QString lineColor3, qreal x31, qreal y31, qreal x32, qreal y32,
                                        QString lineColor4, qreal x41, qreal y41, qreal x42, qreal y42,
                                        bool drawBlock);

    // pixel manipulation
    QVariantList getImageData(qreal sx, qreal sy, qreal sw, qreal sh);
    void putImageData(QVariantList image, qreal dx, qreal dy);
    void createHeatMap(qreal sx, qreal sy, qreal sw, qreal sh, qreal dx, qreal dy);

    void endPainting();

signals:
    void changed();

private:
    void setupPainter();
    void beginPainting();
    void updateShadowBuffer();

    int m_changeTimerId;
    QPainterPath m_path;

    enum DirtyFlag {
        DirtyTransformationMatrix = 0x00001,
        DirtyClippingRegion       = 0x00002,
        DirtyStrokeStyle          = 0x00004,
        DirtyFillStyle            = 0x00008,
        DirtyGlobalAlpha          = 0x00010,
        DirtyLineWidth            = 0x00020,
        DirtyLineCap              = 0x00040,
        DirtyLineJoin             = 0x00080,
        DirtyMiterLimit           = 0x00100,
        MDirtyPen                 = DirtyStrokeStyle
                                    | DirtyLineWidth
                                    | DirtyLineCap
                                    | DirtyLineJoin
                                    | DirtyMiterLimit,
                                    DirtyShadowOffsetX        = 0x00200,
                                    DirtyShadowOffsetY        = 0x00400,
                                    DirtyShadowBlur           = 0x00800,
                                    DirtyShadowColor          = 0x01000,
                                    DirtyGlobalCompositeOperation = 0x2000,
                                    DirtyFont                 = 0x04000,
                                    DirtyTextAlign            = 0x08000,
                                    DirtyTextBaseline         = 0x10000,
                                    AllIsFullOfDirt           = 0xfffff
                                                            };

    struct State {
        State() : flags(0) {}
        QMatrix matrix;
        QPainterPath clipPath;
        QBrush strokeStyle;
        QBrush fillStyle;
        qreal globalAlpha;
        qreal lineWidth;
        Qt::PenCapStyle lineCap;
        Qt::PenJoinStyle lineJoin;
        qreal miterLimit;
        qreal shadowOffsetX;
        qreal shadowOffsetY;
        qreal shadowBlur;
        QColor shadowColor;
        QPainter::CompositionMode globalCompositeOperation;
        QFont font;
        Context2D::TextAlign textAlign;
        Context2D::TextBaseLine textBaseline;
        int flags;
    };

    int baseLineOffset(Context2D::TextBaseLine value, const QFontMetrics &metrics);
    int textAlignOffset(Context2D::TextAlign value, const QFontMetrics &metrics, const QString &string);

    State m_state;
    QStack<State> m_stateStack;
    QPixmap m_pixmap;
    QImage m_shadowbuffer;
    QVector<QRgb> m_shadowColorIndexBuffer;
    QColor m_shadowColorBuffer;
    QPainter m_painter;
    int m_width, m_height;
    bool m_inPaint;
};

#endif
