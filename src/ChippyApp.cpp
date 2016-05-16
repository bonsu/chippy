#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#define debug

#include "Emulator.hpp"

#include <cstdlib>
#include <string>
#include <vector>

using namespace ci;
using namespace ci::app;

const int appDefaultWidth = 640;
const int appDefaultHeight = 320;

void prepareSettings(App::Settings *settings)
{
    settings->setWindowSize(appDefaultWidth, appDefaultHeight);
#ifdef debug
    settings->setTitle("Chippy [Chip-8 Interpreter/Emulator] (Debug Mode)");
#else
    settings->setTitle("Chippy [Chip-8 Interpreter/Emulator]");
#endif
}



class ChippyApp : public App {
public:
    void setup() override;
    
    void keyDown( KeyEvent event ) override;
    void keyUp( KeyEvent event ) override;
    
    void fileDrop( FileDropEvent event ) override;
    
    void resize() override;
    void update() override;
    void draw() override;

private:
    Emulator chipEmulator;
    
    uint8_t texData[32][64][3];
    gl::Texture2dRef screenTexture;
    Rectf textureBounds;
    Rectf drawBounds;
    Font fontName;
    gl::TextureFontRef textureFont;
    
    bool dbgToggleSingleStepMode = false;
    bool dbgSingleStepKeyPressed = false;
    
    void renderDisplayToTexture();
    void renderDisplayToConsole();
};

void ChippyApp::renderDisplayToTexture()
{
    for (int y = 0; y < 32; ++y)
        for (int x = 0; x < 64; ++x)
            if (chipEmulator.display[y][x] == 0)
                texData[y][x][0] = texData[y][x][1] = texData[y][x][2] = 0;
            else
                texData[y][x][0] = texData[y][x][1] = texData[y][x][2] = 255;
    screenTexture->update(texData, GL_RGB, GL_UNSIGNED_BYTE, 0, 64, 32);
}

void ChippyApp::renderDisplayToConsole()
{
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (chipEmulator.display[y][x] == 0)
                console() << " ";
            else
                console() << "*";
        }
        console() << std::endl;
    }
    console() << std::endl;
}

void ChippyApp::setup()
{
    // clear texData
    for (int y = 0; y < 32; ++y)
        for (int x = 0; x < 64; ++x)
                texData[y][x][0] = texData[y][x][1] = texData[y][x][2] = 0;
    // init screenTexture
    screenTexture = gl::Texture2d::create(texData, GL_RGB, 64, 32);
    screenTexture->setMinFilter(GL_NEAREST);
    screenTexture->setMagFilter(GL_NEAREST);
    screenTexture->setTopDown(true);
    textureBounds = screenTexture->getBounds();
    drawBounds = textureBounds.getCenteredFit(getWindowBounds(), true);
    
    // setup font
    fontName = Font("Helvetica", 12);
    textureFont = gl::TextureFont::create(fontName);
    
}

void ChippyApp::keyDown(KeyEvent event)
{
    switch (event.getCode()) {
        case KeyEvent::KEY_1:
            chipEmulator.setKeyPressed(0x1);
            break;
        case KeyEvent::KEY_2:
            chipEmulator.setKeyPressed(0x2);
            break;
        case KeyEvent::KEY_3:
            chipEmulator.setKeyPressed(0x3);
            break;
        case KeyEvent::KEY_4:
            chipEmulator.setKeyPressed(0xC);
            break;
        case KeyEvent::KEY_q:
            chipEmulator.setKeyPressed(0x4);
            break;
        case KeyEvent::KEY_w:
            chipEmulator.setKeyPressed(0x5);
            break;
        case KeyEvent::KEY_e:
            chipEmulator.setKeyPressed(0x6);
            break;
        case KeyEvent::KEY_r:
            chipEmulator.setKeyPressed(0xD);
            break;
        case KeyEvent::KEY_a:
            chipEmulator.setKeyPressed(0x7);
            break;
        case KeyEvent::KEY_s:
            chipEmulator.setKeyPressed(0x8);
            break;
        case KeyEvent::KEY_d:
            chipEmulator.setKeyPressed(0x9);
            break;
        case KeyEvent::KEY_f:
            chipEmulator.setKeyPressed(0xE);
            break;
        case KeyEvent::KEY_z:
            chipEmulator.setKeyPressed(0xA);
            break;
        case KeyEvent::KEY_x:
            chipEmulator.setKeyPressed(0x0);
            break;
        case KeyEvent::KEY_c:
            chipEmulator.setKeyPressed(0xB);
            break;
        case KeyEvent::KEY_v:
            chipEmulator.setKeyPressed(0xF);
            break;
        case KeyEvent::KEY_k:
            dbgSingleStepKeyPressed = true;
            break;
        case KeyEvent::KEY_j:
            dbgToggleSingleStepMode = !dbgToggleSingleStepMode;
    }
}

void ChippyApp::keyUp(KeyEvent event)
{
    switch (event.getCode()) {
        case KeyEvent::KEY_1:
            chipEmulator.setKeyReleased(0x1);
            break;
        case KeyEvent::KEY_2:
            chipEmulator.setKeyReleased(0x2);
            break;
        case KeyEvent::KEY_3:
            chipEmulator.setKeyReleased(0x3);
            break;
        case KeyEvent::KEY_4:
            chipEmulator.setKeyReleased(0xC);
            break;
        case KeyEvent::KEY_q:
            chipEmulator.setKeyReleased(0x4);
            break;
        case KeyEvent::KEY_w:
            chipEmulator.setKeyReleased(0x5);
            break;
        case KeyEvent::KEY_e:
            chipEmulator.setKeyReleased(0x6);
            break;
        case KeyEvent::KEY_r:
            chipEmulator.setKeyReleased(0xD);
            break;
        case KeyEvent::KEY_a:
            chipEmulator.setKeyReleased(0x7);
            break;
        case KeyEvent::KEY_s:
            chipEmulator.setKeyReleased(0x8);
            break;
        case KeyEvent::KEY_d:
            chipEmulator.setKeyReleased(0x9);
            break;
        case KeyEvent::KEY_f:
            chipEmulator.setKeyReleased(0xE);
            break;
        case KeyEvent::KEY_z:
            chipEmulator.setKeyReleased(0xA);
            break;
        case KeyEvent::KEY_x:
            chipEmulator.setKeyReleased(0x0);
            break;
        case KeyEvent::KEY_c:
            chipEmulator.setKeyReleased(0xB);
            break;
        case KeyEvent::KEY_v:
            chipEmulator.setKeyReleased(0xF);
            break;
    }
}

void ChippyApp::fileDrop(FileDropEvent event)
{
    auto file = event.getFile(0);
    chipEmulator.reset();
    if (!chipEmulator.loadBinary(file.string()))
        console() << "could not load " << file.string() << std::endl;
}

void ChippyApp::resize()
{
    drawBounds = textureBounds.getCenteredFit(getWindowBounds(), true);
}

void ChippyApp::update()
{
    // run at 60hz say 60 instructions per second
    
    static double cpuClockSpeedTimer = 0;
    static double t60 = 0;
    double secondsToWait = (double)1/120;
    
    if ((getElapsedSeconds() - cpuClockSpeedTimer) > secondsToWait) {
#ifdef debug
        if (dbgToggleSingleStepMode) {
            if (dbgSingleStepKeyPressed) {
                chipEmulator.cpuCycle();
                if (chipEmulator.drawDisplay) {
                    renderDisplayToTexture();
                    chipEmulator.drawDisplay = false;
                }
            }
            dbgSingleStepKeyPressed = false;
        }
        else {
            chipEmulator.cpuCycle();
            if (chipEmulator.drawDisplay) {
                renderDisplayToTexture();
                chipEmulator.drawDisplay = false;
            }
        }
#else
        chipEmulator.cpuCycle();
        /*if (chipEmulator.drawDisplay) {
            renderDisplayToTexture();
            chipEmulator.drawDisplay = false;
        }*/
        renderDisplayToTexture();
    
#endif
    }
    
    double t60elapsed = getElapsedSeconds() - t60;
    if (t60elapsed > 1) {
        //console() << chipEmulator.statInstructionCount << " Instructions executed in " << t60elapsed << " seconds" << std::endl;
        chipEmulator.statInstructionCount = 0;
        t60 = getElapsedSeconds();
    }
    
    cpuClockSpeedTimer = getElapsedSeconds();
}

void ChippyApp::draw()
{
    gl::setMatricesWindow(getWindowSize());
    
    gl::clear(Color(0, 0, 0));
    gl::draw(screenTexture, drawBounds);

#ifdef debug
    gl::enableAlphaBlending();
    gl::color(ColorA(0.0f, 1.0f, 0.0f, 0.9f));
    std::string debugModeStr = "press J to enable/disable single step mode\npress K to single step";
    float fontNameWidth = textureFont->measureString(debugModeStr).x;
    textureFont->drawString(debugModeStr, vec2(getWindowWidth()-fontNameWidth-10,
                                               getWindowHeight()-textureFont->getDescent()-15));
    gl::disableAlphaBlending();
    gl::color(Color(1, 0, 0));
#endif
}

CINDER_APP(ChippyApp, RendererGl, prepareSettings)