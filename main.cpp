#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <time.h>
#define _USE_MATH_DEFINES

class Agent 
{
    public:
    double x, y, vx, vy;
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

    sf::CircleShape draw()
    {
        sf::CircleShape body(1.f);
        body.move(x,y);
        return body;
    }
};

void setPixelColor(sf::Texture& texture, int x, int y, sf::Color color){
    sf::Image image = texture.copyToImage();
    image.setPixel(x, y, color);
    texture.loadFromImage(image);
} 

int main()
{
    int itercounter = 0;
    int windowWidth = 200, windowHeight = 200;
    int AGENTS_NUMBER = 10;

    sf::Color COLOR_WHITE(255,255,255,255);

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML works!");
    Agent agents[AGENTS_NUMBER];
    
    sf::Texture MTexture;
    MTexture.create(windowWidth, windowHeight);

    srand(time(0));

    for (int i =0; i < AGENTS_NUMBER; i++)
    {
        agents[i] = Agent(rand()%windowWidth, rand()%windowHeight, (((double) rand() / (RAND_MAX)) * 2 - 1), (((double) rand() / (RAND_MAX)) * 2 - 1));
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
            agents[i].move(windowWidth, windowHeight);

            sf::Image image = MTexture.copyToImage();
            std::cout << ((int) agents[i].x) << "\t" << ((int) agents[i].y) << "\n";
            image.setPixel(((int) agents[i].x), ((int) agents[i].y), COLOR_WHITE);
            MTexture.loadFromImage(image);

            //setPixelColor(MTexture, agents[i].x, agents[i].y, COLOR_WHITE);
        }

        //MTexture.copyToImage().saveToFile("/media/rincewind/D/Code/Simulations/render/" + std::to_string(itercounter) + ".png");
        sf::Sprite sprite(MTexture);
        window.draw(sprite);
        window.display();
        itercounter++;
    }

    return 0;
}