#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <time.h>
#include <vector>
#include <random>
#define _USE_MATH_DEFINES

// Preferences and preparations

const int windowWidth = 800, windowHeight = 600;
const int AGENTS_NUMBER = 5000;
const int SENSOR_LENGTH = 7; // MUST be greater than SENSOR_SIZE
const int SENSOR_SIZE = 2; //MUST be even
const double SENSOR_ANGLE = M_PI/4;
const int EDGE_L = 5;
const double TURN_ANGLE = M_PI/8;
const float TURN_RAND = 2.5;
const float EVAPO_RATE = 1.25;

const sf::Color COLOR_WHITE(255,255,255);
const sf::Color COLOR_GREEN(0,255,0,128);

static std::default_random_engine e;
static std::uniform_real_distribution<> dis(0, 1);

float randomValue(float min = -1, float max = 1, bool allowMiddle = false )
{
    float rcoef = dis(e);

    if (!allowMiddle)
    {
        while (rcoef == 0.5)
        {
            rcoef = dis(e);
        }
    }

    return rcoef*(max - min) + min;
}

void setPixels(sf::Image& image, std::vector<std::vector<int>>& buffer, sf::Color color){
    for (int i = 0; i < buffer.size(); i++)
    {
        if ((buffer.at(i).at(0) < image.getSize().x) && (buffer.at(i).at(0) > 0) && (buffer.at(i).at(1) < image.getSize().y) && (buffer.at(i).at(1) > 0))
        {
            image.setPixel(buffer.at(i).at(0), buffer.at(i).at(1), color);
        }
    }
    buffer.clear();
} 

void evaporateImage(sf::Image& img){
    for (int i = 0; i < img.getSize().x; i++)
    {
        for (int j = 0; j < img.getSize().y; j++)
        {
            sf::Color c = img.getPixel(i,j);
            c = sf::Color(c.r - (c.r >= EVAPO_RATE) * EVAPO_RATE, c.g - (c.g >= EVAPO_RATE) * EVAPO_RATE, c.b - (c.b >= EVAPO_RATE) * EVAPO_RATE);
            img.setPixel(i,j,c);
        }
    }
}

sf::Color getPixelColor(sf::Image& image, int x, int y){
    if ((x < image.getSize().x) && (x > 0) && (y < image.getSize().y) && y > 0)
    {
        return image.getPixel(x,y);
    }
    else
    {
        return sf::Color(0,0,0);
    }
}

float getAvAreaValue(sf::Image& image, int x, int y, int a){
    int avg = 0;
    for (int i = -a/2; i <= a/2; i++){
        for (int j = -a/2; j <= a/2; j++){
            sf::Color colr = getPixelColor(image, x + i, y + j);
            avg = avg + (colr.r + colr.g + colr.b)/3;

            // if ((x+i < image.getSize().x) && (x+i > 0) && (y+j < image.getSize().y) && (y+j > 0)){
            //     //std::cout << x + i << " " << y + j << " " << image.getSize().x << " " << image.getSize().y << "\n";
            //     image.setPixel(x+i, y+j, COLOR_GREEN);
            // }

            //draw sensors, useful for debug
        }
    }

    return avg/pow(a,2);
}


class Agent 
{
    public:
    float x, y, vx, vy;
    int bcounter = 0;

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
        
        float pvx = vx, pvy = vy;
        int rsd = randomValue()*TURN_RAND;

        vx = pvx*cos(rsd*TURN_ANGLE) - pvy*sin(rsd*TURN_ANGLE);
        vy = pvy*cos(rsd*TURN_ANGLE) + pvx*sin(rsd*TURN_ANGLE);

        x += vx;
        y += vy;
    }

    void search(sf::Image& image)
    {
        int lx, ly, srx, sry, slx, sly;
        float cvl, cvr;

        lx = SENSOR_LENGTH * vx / sqrt(pow(vx, 2) + pow(vy, 2));
        ly = SENSOR_LENGTH * vy / sqrt(pow(vx, 2) + pow(vy, 2));

        slx = int(x + lx*cos(SENSOR_ANGLE) - ly*sin(SENSOR_ANGLE));
        sly = int(y + ly*cos(SENSOR_ANGLE) + lx*sin(SENSOR_ANGLE));

        srx = int(x + lx*cos(-SENSOR_ANGLE) - ly*sin(-SENSOR_ANGLE));
        sry = int(y + ly*cos(-SENSOR_ANGLE) + lx*sin(-SENSOR_ANGLE));

        cvl = getAvAreaValue(image, slx, sly, SENSOR_SIZE);
        cvr = getAvAreaValue(image, srx, sry, SENSOR_SIZE);

        float pvx = vx, pvy = vy;

        if ((cvl == cvr) && (cvl*cvr !=0))
        {
            
            int rsd = randomValue()*TURN_RAND;

            vx = pvx*cos(rsd*TURN_ANGLE) - pvy*sin(rsd*TURN_ANGLE);
            vy = pvy*cos(rsd*TURN_ANGLE) + pvx*sin(rsd*TURN_ANGLE);
        }
        else
        { if (cvl < cvr )
            {
                vx = pvx*cos(-TURN_ANGLE) - pvy*sin(-TURN_ANGLE);
                vy = pvy*cos(-TURN_ANGLE) + pvx*sin(-TURN_ANGLE);
            }
        else
        {   if (cvl > cvr )
        {
                vx = (pvx*cos(TURN_ANGLE) - pvy*sin(TURN_ANGLE));
                vy = (pvy*cos(TURN_ANGLE) + pvx*sin(TURN_ANGLE));
            }   
        }
        }
        
    }

};


int main()
{
    int itercounter = 0;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "ITS ALIIIIVE!");
    std::vector<Agent> agents;

    std::vector<std::vector<int>> PixelBuffer;
    
    sf::Image MImage;
    MImage.create(windowWidth, windowHeight);

    int const c_r = 100;

    for (int i =0; i < AGENTS_NUMBER; i++)
    {
        agents.push_back(Agent(windowWidth / 2 + c_r*sin(2*M_PI*i/AGENTS_NUMBER),windowHeight/2 + c_r*cos(2*M_PI*i/AGENTS_NUMBER),randomValue(), randomValue()));
        
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

        evaporateImage(MImage);
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