#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <time.h>
#include <vector>
#define _USE_MATH_DEFINES

// Preferences and preparations

const int windowWidth = 200, windowHeight = 200;
const int AGENTS_NUMBER = 100;
const int SENSOR_LENGTH = 4; // MUST be greater than SENSOR_SIZE
const int SENSOR_SIZE = 2; //MUST be even
const double SENSOR_ANGLE = M_PI/4;
const int EDGE_L = 5;
const double TURN_ANGLE = SENSOR_ANGLE;
const int EVAPO_RATE = 2;

const sf::Color COLOR_WHITE(255,255,255);
const sf::Color COLOR_GREEN(0,255,0,128);


void setPixels(sf::Image& image, std::vector<std::vector<int>>& buffer, sf::Color color){
    for (int i = 0; i < buffer.size(); i++)
    {
        if ((buffer.at(i).at(0) < image.getSize().x) && (buffer.at(i).at(0) > 0) && (buffer.at(i).at(1) < image.getSize().y) && (buffer.at(i).at(1) > 0))
        {
            image.setPixel(buffer.at(i).at(0), buffer.at(i).at(1), color);
            //std::cout << "Error is not here\n";
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

sf::Color getPixelColor(sf::Image image, int x, int y){
    if ((x < image.getSize().x) && (x > 0) && (y < image.getSize().y) && y > 0)
    {
        return image.getPixel(x,y);
    }
    else
    {
        return sf::Color(0,0,0);
    }
}

float getAvAreaValue(sf::Image image, int x, int y, int a){
    int avg = 0;
    for (int i = -a/2; i < a/2; i++){
        for (int j = -a/2; j < a/2; j++){
            avg += getPixelColor(image, x + i, y + j).r;

            /*if ((x+i < image.getSize().x) && (x+i > 0) && (y+j < image.getSize().y) && (y+j > 0)){
                //std::cout << x + i << " " << y + j << " " << image.getSize().x << " " << image.getSize().y << "\n";
                image.setPixel(x+i, y+j, COLOR_GREEN);
            }*/
        }
    }
    /*std::vector<std::vector<int>> dbgbox = {{x - a/2, y - a/2},{x - a/2, y + a/2},{x + a/2, y - a/2},{x + a/2, y + a/2}};

    std::cout << x - a/2 << " " << y - a/2 << "\n";
    setPixels(image, dbgbox, COLOR_WHITE);*/

    return avg/pow(a,2);
}

//Agent class

class Agent 
{
    public:
    double x, y, vx, vy;
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

        x += vx;
        y += vy;
    }

    void search(sf::Image& image)
    {
        int lx, ly, srx, sry, slx, sly;
        float cvl, cvr;

        lx = SENSOR_LENGTH * vx / sqrt(pow(vx, 2) + pow(vy, 2));
        ly = SENSOR_LENGTH * vy / sqrt(pow(vx, 2) + pow(vy, 2));
        //std::cout << sqrt(pow(lx,2) + pow(ly,2)) << "\n";
        //std::cout << lx << " " << ly << "\n";

        slx = int(x + lx*cos(SENSOR_ANGLE) - ly*sin(SENSOR_ANGLE));
        sly = int(y + ly*cos(SENSOR_ANGLE) + lx*sin(SENSOR_ANGLE));

        srx = int(x + lx*cos(-SENSOR_ANGLE) - ly*sin(-SENSOR_ANGLE));
        sry = int(y + ly*cos(-SENSOR_ANGLE) + lx*sin(-SENSOR_ANGLE));

        /*std::vector <std::vector<int>> dbgpixls =  {{slx, sly}, {srx, sry}};
        setPixels(image, dbgpixls, COLOR_GREEN);*/

        cvl = getAvAreaValue(image, slx, sly, SENSOR_SIZE);
        cvr = getAvAreaValue(image, srx, sry, SENSOR_SIZE);

        if ((cvl == cvr) && (cvl*cvr !=0))
        {
            srand(time(0));
            int rsd = int(2*rand()/RAND_MAX - 1);

            vx = vx*cos(rsd*TURN_ANGLE) - vy*sin(rsd*TURN_ANGLE);
            vy = vy*cos(rsd*TURN_ANGLE) + vx*sin(rsd*TURN_ANGLE);
        }
        else
        { if (cvl < cvr )
            {
                vx = vx*cos(-TURN_ANGLE) - vy*sin(-TURN_ANGLE);
                vy = vy*cos(-TURN_ANGLE) + vx*sin(-TURN_ANGLE);
            }
        else
        {   if (cvl > cvr )
        {
                vx = (vx*cos(TURN_ANGLE) - vy*sin(TURN_ANGLE)) * vx / sqrt(pow(vx,2) + pow(vy,2));
                vy = (vy*cos(TURN_ANGLE) + vx*sin(TURN_ANGLE)) * vx / sqrt(pow(vx,2) + pow(vy,2));
            }   
        }
        }
        
        /*vx += vx* (abs(vx) < 1) * 0.1;
        vy += vy* (abs(vy) < 1) * 0.1;*/
    }

};

// Main function

int main()
{
    int itercounter = 0;

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