#ifndef SCREENMGR_H
#define SCREENMGR_H

#define SCREEN_SWITCH_BUTTON 1

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
            int targerScreen = screens[current]->handleInput(button);
            if (targerScreen >= 0 && targerScreen < screensCount) {
                setScreen(targerScreen);
            }

            if (button == SCREEN_SWITCH_BUTTON) {
                nextScreen();
            }
        }
        
    private:
        Screen** screens;
        int screensCount;
        int current;
};

#endif