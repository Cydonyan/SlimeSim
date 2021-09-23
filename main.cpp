#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <time.h>
#include <vector>
#define _USE_MATH_DEFINES

class Agent 
{
    public:
    double x, y, vx, vy;
    double angle = M_PI/6;

    Agent()
    {
        x = 0;
        y = 0;
        vx = 0;
        vy = 0;
    }
    Agent(float initial_x, float initial_y, float initial_vx, float initial_vy)
    {
        x = initial_x;
        y = initial_y;
        vx = initial_vx;
        vy = initial_vy;
    }

    void move(int windowWidth, int windowHeight)
    {
        if ((x > windowWidth - 5) || (x < 5))
        {
            vx = -vx;
        }
        if ((y > windowHeight - 5) || (y < 5))
        {
            vy = -vy;
        }

        x += vx;
        y += vy;
    }

    void search()
    {

    }

    sf::CircleShape draw()
    {
        sf::CircleShape body(1.f);
        body.move(x,y);
        return body;
    }
};

void setPixels(sf::Texture& texture, std::vector<std::vector<int>>& buffer, sf::Color color){
    sf::Image image = texture.copyToImage();

    for (int i = 0; i < buffer.size(); i++)
    {
        image.setPixel(buffer.at(i).at(0), buffer.at(i).at(1), color);
    }
    //image.setPixel(i.at(0), i.at(0), color);
    texture.loadFromImage(image);
    buffer.clear();
} 

/* sf::Color getPixelColor(sf::Texture& texture, int x, int y){
    sf::Image image = texture.copyToImage();
    return image.getPixel(x,y);
} */

int main()
{
    int itercounter = 0;
    int windowWidth = 1000, windowHeight = 1000;
    int AGENTS_NUMBER = 500;

    sf::Color COLOR_WHITE(255,255,255,255);

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML works!");
    std::vector<Agent> agents;

    std::vector<std::vector<int>> PixelBuffer;
    
    sf::Texture MTexture;
    MTexture.create(windowWidth, windowHeight);

    srand(time(0));

    for (int i =0; i < AGENTS_NUMBER; i++)
    {
        agents.push_back(Agent(rand()%windowWidth, rand()%windowHeight, (((double) rand() / (RAND_MAX)) * 2 - 1), (((double) rand() / (RAND_MAX)) * 2 - 1)));
    }


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        for (int i = 0; i < AGENTS_NUMBER; i++)
        {
            agents.at(i).move(windowWidth, windowHeight);
            PixelBuffer.push_back({((int) agents.at(i).x), ((int) agents.at(i).y)});
        }

        setPixels(MTexture, PixelBuffer, COLOR_WHITE);
        
        sf::Sprite sprite(MTexture);
        window.draw(sprite);
        window.display();
        itercounter++;
    }

    return 0;
}