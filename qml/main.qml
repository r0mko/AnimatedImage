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
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

import AnimatedImageTexture 1.0

Window {
    id: root
    width: 620
    height: 680
    visible: true
    color: "#eee"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Item {
            id: _container
            width: 600
            height: 600

            Loader {
                anchors.fill: parent
                property color color1: "#333"
                property color color2: "#444"
                source: "Checker_Qt%1.qml".arg(qtVersionMajor)
            }

            AnimatedImage {
                id: _image
                t2: t1 + _blur.value
                blur: _blurCheckbox.checked
                anchors.centerIn: parent
                fileNamePattern: "assets/elephant_*.png"
            }

            PropertyAnimation {
                id: _animation
                target: _image
                running: true
                loops: Animation.Infinite
                property: "t1"
                from: 0.0
                to: 1.0
                easing.type: Easing.Linear
                duration: _animationDuration.value
            }

            Item {
                id: _textContainer
                width: 120
                height: 180
                visible: false
                property int currentFigure: 0

                Text {
                    id: _text
                    anchors.centerIn: parent
                    color: "#fff"
                    font.pixelSize: 150
                    text: "0"
                }

                function grabImage() {
                    _text.text = String(currentFigure)
                    _textContainer.grabToImage(function(result) {
                        _image.appendFrame(result);
                        createNextFrame();
                    });
                }

                function createNextFrame() {
                    if (currentFigure < 9) {
                        ++currentFigure;
                        grabImage();
                    }
                }
            }

        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#eee"

            RowLayout {
                anchors.centerIn: parent

                Button {
                    text: checked ? "Elephant mode" : "Numbers mode"
                    checkable: true
                    onClicked: {
                        if (checked) {
                            _image.beginCreateAnimation(_textContainer.width, _textContainer.height, 10)
                            _textContainer.grabImage()
                        } else {
                            _image.reset()
                        }
                    }
                }

                Label {
                    text: "Blur amount:"
                }


                Slider {
                    id: _blur
                    from: 0.0
                    to: 1.0

                    Label {
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: -16
                        text: _blur.value.toFixed(2)
                    }
                }

                CheckBox {
                    id: _blurCheckbox
                    checked: true
                    text: "Blur:"
                }

                Label {
                    text: "Duration:"
                }

                Rectangle {
                    border.color: "#000"
                    border.width: 1
                    radius: 3
                    color: "#fff"
                    width: _animationDuration.width + 20
                    height: _animationDuration.height + 10

                    TextInput {
                        id: _animationDuration
                        anchors.centerIn: parent
                        property int value: 1500
                        validator: IntValidator {}
                        text: value
                        onAccepted: {
                            _animation.stop()
                            value = text
                            _animation.restart()
                            focus = false
                        }
                    }
                }

                Label {
                    text: "ms"
                }
            }
        }
    }
}
