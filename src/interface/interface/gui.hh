#pragma once

class GUI
{
public:
    void init();
    void render();
    void renderLogs();
    void shutdown();

private:
    bool m_open = true;
    bool m_inited = false;
    int m_tab = 0;
};

extern GUI* ngui;
