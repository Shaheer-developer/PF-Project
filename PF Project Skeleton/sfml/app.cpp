#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/grid.h"
#include "../core/switches.h"
#include "../core/io.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;

// ============================================================================
// APP.CPP - Implementation of SFML application (NO CLASSES)
// ============================================================================

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES FOR APP STATE
// ----------------------------------------------------------------------------
static sf::RenderWindow* g_window = nullptr;
static sf::Font g_font;

// View for camera (panning/zoom)
static sf::View g_camera;

// Simulation state
static bool g_isPaused = false;
static bool g_isStepMode = false;

// Mouse state
static bool g_isDragging = false;
static int g_lastMouseX = 0;
static int g_lastMouseY = 0;

// Grid rendering parameters
static float g_cellSize = 40.0f;
static float g_gridOffsetX = 50.0f;
static float g_gridOffsetY = 50.0f;


sf::Texture horizontal_t, vertical_t, trackCross, trackLeft, trackRight;
sf::Texture switchOpen, switchClosed;
sf::Texture spawn, dest, safety;
sf::Texture train_up, train_down, train_left, train_right;
sf::Texture indicator_g, indicator_y, indicator_r;

void loadTexture(sf::Texture &tex, string filename)
{
    string path = "Sprites/GridTiles/" + filename;
    if (!tex.loadFromFile(path))
    {
        cerr << "Error loading: " << path << endl;
        tex.create(32, 32);
    }
    tex.setSmooth(true);
}
// ----------------------------------------------------------------------------
// INITIALIZATION
// ----------------------------------------------------------------------------
// This function will initialize the SFML application window and resources.
// It creates a render window with a specified size and title, sets the
// framerate limit, attempts to load a font file for text rendering, and
// initializes the camera view. Returns true on success, false on failure.
// This should be called once at the start of the application before entering
// the main loop.
// ----------------------------------------------------------------------------
bool initializeApp() {
     g_window = new sf::RenderWindow(sf::VideoMode(1280, 720), "Switchback Rails");
    g_window->setFramerateLimit(60);

    cout << "Loading Textures..." << endl;

    // loading texture
    loadTexture(horizontal_t, "track.png");               
    loadTexture(vertical_t, "track_vertical.png");      
    loadTexture(trackCross, "track_cross.png");      
    loadTexture(trackLeft, "track_tilt_left.png");   
    loadTexture(trackRight, "track_tilt_right.png"); 

    loadTexture(spawn, "tile_spawn_blue.png");    
    loadTexture(dest, "tile_destination_red.png"); 
    loadTexture(safety, "tile_safety.png");        

    loadTexture(switchClosed, "switch_closed.png"); 
    loadTexture(switchOpen, "switch_open.png");     

    // Trains
    loadTexture(train_up, "up_train.png");
    loadTexture(train_down, "down_train.png");
    loadTexture(train_left, "left_train.png");
    loadTexture(train_right, "right_train.png");

    // Indicators 
    loadTexture(indicator_g, "green_indicator.png");
    loadTexture(indicator_y, "yellow_indicator.png");
    loadTexture(indicator_r, "red_indicator.png");

    float mapW = cols * g_cellSize;
    float mapH = rows * g_cellSize;
    g_camera.setSize(1280, 720);
    g_camera.setCenter(mapW / 2.0f, mapH / 2.0f);
    g_window->setView(g_camera);

    return true;
}


void drawFitSprite(sf::Texture &tex, float x, float y)
{
    sf::Sprite sprite;
    sprite.setTexture(tex);

    sf::Vector2u size = tex.getSize();

    if (size.x == 0 || size.y == 0)
        return;

    // calculating scale factor
    float scaleX = g_cellSize / (float)size.x;
    float scaleY = g_cellSize / (float)size.y;

    sprite.setScale(scaleX, scaleY);
    sprite.setPosition(x, y);

    g_window->draw(sprite);
}
// ----------------------------------------------------------------------------
// MAIN RUN LOOP
// ----------------------------------------------------------------------------
// This function will run the main application loop. It handles event processing,
// simulation updates, and rendering. The loop continues while the window is open.
// It processes SFML events (window close, keyboard input, mouse input), updates
// the simulation at a fixed interval (2 ticks per second) when not paused,
// checks if the simulation is complete, and renders the current frame. Keyboard
// controls: SPACE to pause/resume, PERIOD to step one tick, ESC to exit. The
// loop exits when the window is closed or ESC is pressed.
// ----------------------------------------------------------------------------
void runApp() {
     sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timePerTick = sf::seconds(0.5f);

    while (g_window->isOpen())
    {
        //events like keyboard button pressed and other mouse scrollig
        sf::Event event;
        while (g_window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                g_window->close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    g_window->close();
                    writeMetrics();
                }
                else if (event.key.code == sf::Keyboard::Space)
                {
                    g_isPaused = !g_isPaused;
                }
                else if (event.key.code == sf::Keyboard::Period)
                {
                    updateTick();
                }
            }
            // mouse zoom
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta > 0)
                    g_camera.zoom(0.9f);
                else
                    g_camera.zoom(1.1f);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle)
            {
                g_isDragging = true;
                g_lastMouseX = event.mouseButton.x;
                g_lastMouseY = event.mouseButton.y;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle)
            {
                g_isDragging = false;
            }
            if (event.type == sf::Event::MouseMoved && g_isDragging)
            {
                float zoom = g_camera.getSize().x / g_window->getSize().x;
                g_camera.move(-(event.mouseMove.x - g_lastMouseX) * zoom, -(event.mouseMove.y - g_lastMouseY) * zoom);
                g_lastMouseX = event.mouseMove.x;
                g_lastMouseY = event.mouseMove.y;
            }
        }

        if (!g_isPaused)
        {
            timeSinceLastUpdate += clock.restart();
            while (timeSinceLastUpdate > timePerTick)
            {
                timeSinceLastUpdate -= timePerTick;
                updateTick();
            }
        }
        else
        {
            clock.restart();
        }


        g_window->clear(sf::Color(20, 20, 20)); //background
        g_window->setView(g_camera);

        //drawing grid with sprites
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                char tile = grid[r][c];
                float px = c * g_cellSize;
                float py = r * g_cellSize;

                if (tile == '-')
                    drawFitSprite(horizontal_t, px, py);
                else if (tile == '|')
                    drawFitSprite(vertical_t, px, py);
                else if (tile == '/')
                    drawFitSprite(trackLeft, px, py);
                else if (tile == '\\')
                    drawFitSprite(trackRight, px, py);
                else if (tile == '+')
                    drawFitSprite(trackCross, px, py);
                else if (tile == 'S')
                    drawFitSprite(spawn, px, py);
                else if (tile == 'D')
                    drawFitSprite(dest, px, py);
                else if (tile == '=')
                    drawFitSprite(safety, px, py);
                else if (tile >= 'A' && tile <= 'Z')
                {
                    //swtich logic and transition
                    int idx = tile - 'A';
                    if (switchState[idx] == 0)
                        drawFitSprite(switchClosed, px, py);
                    else
                        drawFitSprite(switchOpen, px, py);
                }
            }
        }

        //drawing trains
        for (int i = 0; i < MAX_TRAINS; i++)
        {
            if (trainState[i] == TRAIN_STATE_ACTIVE)
            {
                float px = train_x[i] * g_cellSize;
                float py = train_y[i] * g_cellSize;

                switch (currDir[i])
                {
                case 0:
                    drawFitSprite(train_up, px, py);
                    break;
                case 1:
                    drawFitSprite(train_right, px, py);
                    break;
                case 2:
                    drawFitSprite(train_down, px, py);
                    break;
                case 3:
                    drawFitSprite(train_left, px, py);
                    break;
                default:
                    drawFitSprite(train_right, px, py);
                    break;
                }
            }
        }

        g_window->display();
    }
}

// ----------------------------------------------------------------------------
// CLEANUP
// ----------------------------------------------------------------------------
// This function will clean up all resources and close the application window.
// It deletes the render window object and sets the pointer to nullptr. This
// should be called once at the end of the application before exiting to ensure
// proper resource cleanup.
// ----------------------------------------------------------------------------
void cleanupApp() {
    if (g_window)
    {
        delete g_window;
        g_window = nullptr;
    }
}
