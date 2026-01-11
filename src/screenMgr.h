#ifndef SCREENMGR_H
#define SCREENMGR_H

#define SCREEN_SWITCH_CODE -2

#include "Screen.h"

class ScreenManager {
    public:
        ScreenManager(Screen** screens, int screensCount)
            : screens(screens), screensCount(screensCount), current(0) {}
        
        // setting given screen as current
        void setScreen(int id) {
            if (id < 0 || id >= screensCount) return;

            screens[current]->onExit();
            current = id;
            screens[current]->onEnter();
        }

        // skipping to next screen (in circles)'
        void nextScreen() {
            int targetId = current + 1;
            if (targetId >= screensCount) {targetId = 0;}
            setScreen(targetId);
        }

        // update logic
        void update(int dt) {
            screens[current]->update(dt);
        }

        // rendering
        void render() {
            screens[current]->render();
        }

        // buttons input handling
        void handleInput(int button) {
            int targetScreen = screens[current]->handleInput(button);
            if (targetScreen >= 0 && targetScreen < screensCount) {
                setScreen(targetScreen);
            }

            if (targetScreen == SCREEN_SWITCH_CODE) {
                nextScreen();
            }
        }
        
    private:
        Screen** screens;
        int screensCount;
        int current;
};

#endif