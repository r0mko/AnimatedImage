import QtQuick

ShaderEffect {
    id: tileBackground
    property real tileSize: 16
    property color color2: parent.color2
    property color color1: parent.color1

    property size pixelSize: Qt.size(width / tileSize, height / tileSize);

    fragmentShader: "../shaders/checker.frag.qsb"
}
