import QtQuick 2.15

ShaderEffect {
    id: tileBackground
    property real tileSize: 16
    property color color1: parent.color1
    property color color2: parent.color2

    property size pixelSize: Qt.size(width / tileSize, height / tileSize);

    fragmentShader: "
#version 400 core

in highp vec2 qt_TexCoord0;
uniform highp vec2 pixelSize;
uniform lowp vec4 color1;
uniform lowp vec4 color2;
out lowp vec4 fragColor;

void main()
{
    mediump vec2 tc = sign(sin(3.14159265358979323846 * qt_TexCoord0 * pixelSize));
    if (tc.x != tc.y)
        fragColor = color1;
    else
        fragColor = color2;

}
"
}
