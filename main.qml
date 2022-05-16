/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.15
import QtQuick.Window 2.15

import AnimatedImageTexture 1.0

Window {
    id: root
    width: 600
    height: 600
    visible: true

    // The checkers background
//    ShaderEffect {
//        id: tileBackground
//        anchors.fill: parent

//        property real tileSize: 16
//        property color color1: Qt.rgba(0.9, 0.9, 0.9, 1);
//        property color color2: Qt.rgba(0.85, 0.85, 0.85, 1);

//        property size pixelSize: Qt.size(width / tileSize, height / tileSize);

//        fragmentShader: "#version 320 es

//layout(std140, binding = 0) uniform buf {
//    mediump mat4 qt_Matrix;
//    mediump float qt_Opacity;

//    lowp vec4 color1;
//    lowp vec4 color2;
//    highp vec2 pixelSize;
//} ubuf;

//layout(location = 0) in highp vec2 qt_TexCoord0;
//layout(location = 0) out lowp vec4 fragColor;

//void main()
//{
//    mediump vec2 tc = sign(sin(3.14159265358979323846 * qt_TexCoord0 * ubuf.pixelSize));
//    if (tc.x != tc.y)
//        fragColor = ubuf.color1;
//    else
//        fragColor = ubuf.color2;
//}
//"
//    }

    ShaderEffect {
        id: tileBackground
        anchors.fill: parent
        property real tileSize: 16
        property color color1: Qt.rgba(0.9, 0.9, 0.9, 1);
        property color color2: Qt.rgba(0.85, 0.85, 0.85, 1);

        property size pixelSize: Qt.size(width / tileSize, height / tileSize);


        fragmentShader: "#version 400 core

in highp vec2 qt_TexCoord0;
uniform highp vec2 pixelSize;
out lowp vec4 fragColor;

void main()
{
    mediump vec2 tc = sign(sin(3.14159265358979323846 * qt_TexCoord0 * pixelSize));
    if (tc.x != tc.y)
        fragColor = vec4(0.7, 0.7, 0.7, 1.0);
    else
        fragColor = vec4(0.85, 0.85, 0.85, 1.0);

}
"
    }
//    Rectangle {
//        anchors.fill: parent
//        color: "#333"
//    }

    AnimatedImage {
        id: _image
        anchors.fill: parent
        anchors.margins: 10
        fileNamePattern: "assets/elephant_*.png"
    }

//    Rectangle {
//        id: labelFrame
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.bottom: parent.bottom
//        anchors.margins: -10
//        radius: 12
//        color: "#888"
//        border.color: "black"
////        opacity:
//        anchors.fill: _slider
////        width: _slider.width

//    }

    SequentialAnimation {
        running: true
        loops: Animation.Infinite

        PropertyAnimation {
            target: _image
            property: "t"
            from: 0.0
            to: 1.0
            easing.type: Easing.Linear
            duration: 2000
        }
//        PropertyAnimation {
//            target: renderer
//            property: "t"
//            from: 1.0
//            to: 0.0
//            easing.type: Easing.Linear
//            duration: 1333
//        }
//        PauseAnimation {
//            duration: 500

//        }
    }

//    Slider {
//        id: _slider

//        from: 0.0
//        to: 1.0
//        value: 0
////        anchors.centerIn: parent
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 30
////        scale: 2.0
//        width: renderer.width - 60
//    }



//    Text {
//        id: label
//        anchors.bottom: renderer.bottom
//        anchors.left: renderer.left
//        anchors.right: renderer.right
//        anchors.margins: 20
//        wrapMode: Text.WordWrap
//        text: "The blue rectangle with the vintage 'Q' is an FBO, rendered by the application on the scene graph rendering thread. The FBO is managed and displayed using the QQuickFramebufferObject convenience class."
//    }

    Component.onCompleted: {
        root.show()
    }

}
