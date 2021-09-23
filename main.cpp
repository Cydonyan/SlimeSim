#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <time.h>
#include <vector>
#define _USE_MATH_DEFINES

// Preferences and preparations

const int windowWidth = 1000, windowHeight = 1000;
const int AGENTS_NUMBER = 500;
const int SENSOR_LENGTH = 10;
const double SENSOR_ANGLE = M_PI/4;
const int EDGE_L = 5;

void setPixels(sf::Image& image, std::vector<std::vector<int>>& buffer, sf::Color color){
    for (int i = 0; i < buffer.size(); i++)
    {
        if ((0 < buffer.at(i).at(0) < image.getSize().x) && (0 < buffer.at(i).at(1) < image.getSize().y))
        {
            image.setPixel(buffer.at(i).at(0), buffer.at(i).at(1), color);
            //std::cout << "Error is not here\n";
        }
    }
    buffer.clear();
} 

sf::Color getPixelColor(sf::Image image, int x, int y){
    if ((0 < x < image.getSize().x) && (0 < y < image.getSize().y))
    {
        std::cout << "Error is here\n";
        std::cout << x << " " << y << "\n";
        return image.getPixel(x,y);
    }
    else
    {
        return sf::Color(0,0,0);
    }
}

//Agent class

class Agent 
{
    public:
    double x, y, vx, vy;
    double a = SENSOR_ANGLE;

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
        if ((x > windowWidth - EDGE_L) || (x < EDGE_L))
        {
            vx = -vx;
        }
        if ((y > windowHeight - EDGE_L) || (y < EDGE_L))
        {
            vy = -vy;
        }

        x += vx;
        y += vy;
    }

    void search(sf::Image& image)
    {
        float lx, ly, srx, sry, slx, sly;
        sf::Color csl, csr, csc;
        float cvl, cvr, cvc;

        lx = SENSOR_LENGTH * vx / (pow(2, vx) + pow(2, vy));
        ly = SENSOR_LENGTH * vy / (pow(2, vx) + pow(2, vy));

        slx = int(x + lx*cos(a) - ly*sin(a));
        sly = int(y + ly*cos(a) + lx*sin(a));

        srx = int(x + lx*cos(-a) - ly*sin(-a));
        sry = int(y + ly*cos(-a) + lx*sin(-a));

        std::cout << "If you won't see next line you're fucked\n";
        csl = getPixelColor(image, slx, sly);
        csr = getPixelColor(image, srx, sry);
        csc = getPixelColor(image, lx, ly);

        std::cout << "It's OK\n";

        cvl = (csl.r + csl.g + csl.b)/3;
        cvr = (csr.r + csr.g + csr.b)/3;
        cvc = (csc.r + csc.g + csc.b)/3;

        if (cvl == cvr)
        {
            //turn randomly
        }
        else
        { if (cvl > cvl )
            {
                vx = vx*cos(-a) - vy*sin(-a);
                vy = vy*cos(-a) + vx*sin(-a);
            }
        else
        {   if (cvl < cvl )
        {
                vx = vx*cos(a) - vy*sin(a);
                vy = vy*cos(a) + vx*sin(a);
            }   
        }
        }
                
    }

    sf::CircleShape draw()
    {
        sf::CircleShape body(1.f);
        body.move(x,y);
        return body;
    }
};

// Main function

int main()
{
    int itercounter = 0;

    sf::Color COLOR_WHITE(255,255,255,255);

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML works!");
    std::vector<Agent> agents;

    std::vector<std::vector<int>> PixelBuffer;
    
    sf::Image MImage;
    MImage.create(windowWidth, windowHeight);

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
            agents.at(i).search(MImage);
            agents.at(i).move(windowWidth, windowHeight);
            PixelBuffer.push_back({((int) agents.at(i).x), ((int) agents.at(i).y)});
        }

        setPixels(MImage, PixelBuffer, COLOR_WHITE);
        
        sf::Texture MTexture;
        MTexture.loadFromImage(MImage); 
        sf::Sprite sprite(MTexture);
        window.draw(sprite);
        window.display();
        itercounter++;
    }

    return 0;
}