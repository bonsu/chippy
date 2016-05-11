#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Emulator.hpp"

#include <cstdlib>

using namespace ci;
using namespace ci::app;

#define debug

class ChippyApp : public App {
public:
    void setup() override;
    void keyDown( KeyEvent event ) override;
    void keyUp( KeyEvent event ) override;
    void update() override;
    void draw() override;

private:
    Emulator chipEmulator;
    
    const std::string testBinary = "/Users/bonsu/dev/Chippy/xcode/build/Debug/Chip8 Picture.ch8";
//    const std::string testBinary = "/Users/bonsu/dev/Chippy/xcode/build/Debug/test-rnd.ch8";
//    const std::string testBinary = "/Users/bonsu/dev/Chippy/xcode/build/Debug/test-maze.ch8";
//    const std::string testBinary = "/Users/bonsu/dev/Chippy/xcode/build/Debug/test-blinky.ch8";
    
    uint8_t texData[32][64][3];
    gl::Texture2dRef screenTexture;
    Rectf textureBounds;
    Rectf drawBounds;
    
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
    // init screenTexture
    screenTexture = gl::Texture2d::create(texData, GL_RGB, 64, 32);
    screenTexture->setMinFilter(GL_NEAREST);
    screenTexture->setMagFilter(GL_NEAREST);
    screenTexture->setTopDown(true);
    textureBounds = screenTexture->getBounds();
    drawBounds = textureBounds.getCenteredFit(getWindowBounds(), true);
    assert(chipEmulator.loadBinary(testBinary) == true);
    
}

void ChippyApp::keyDown( KeyEvent event )
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
    }
}

void ChippyApp::keyUp( KeyEvent event )
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

void ChippyApp::update()
{
    static unsigned long updateLoop = 0;
    
    
    // run at 60hz say 60 instructions per second
    
    static double timer = 0;
    static double t60 = 0; // get reset after every second;
    double secondsToWait = (double)1/120;
    
    
    if ((getElapsedSeconds() - timer) > secondsToWait) {
        chipEmulator.cpuCycle();
    
        if (chipEmulator.drawDisplay) {
            renderDisplayToTexture();
#ifdef debug
          //  renderDisplayToConsole();
#endif
            chipEmulator.drawDisplay = false;
        }
        
    }
    
    double t60elapsed = getElapsedSeconds() - t60;
    if (t60elapsed > 1) {
        console() << chipEmulator.statInstructionCount << " Instructions executed in " << t60elapsed << " seconds" << std::endl;
        chipEmulator.statInstructionCount = 0;
        t60 = getElapsedSeconds();
    }
    
    timer = getElapsedSeconds();
    updateLoop++;
}

void ChippyApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
#ifdef debug
#endif
    
    
    gl::draw(screenTexture, drawBounds);
}

CINDER_APP( ChippyApp,
            RendererGl,
            [&]( App::Settings *settings ) { settings->setWindowSize( 640, 320 ); })