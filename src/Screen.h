#ifndef SCREEN_H
#define SCREEN_H

class Screen {
    public:
        virtual ~Screen() {}

        virtual void onEnter() {} // called while entering screen
        virtual void onExit() {} // called while exiting screen

        virtual void update(int dt) = 0; // called regylarly to update screens logic
        virtual void render() = 0; // caled regularly to render elements on the screen

        virtual int handleInput(int button) { return -1; } // called when a button is pressed; it can return what screen it wants to jump to
};

#endif
