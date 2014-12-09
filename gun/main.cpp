#include <myo/myo.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace std;

Mix_Chunk *gun = NULL;
Mix_Chunk *charge = NULL;

int init()
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }
    
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;
    }
    
    gun = Mix_LoadWAV( "gun.wav" );
    charge = Mix_LoadWAV( "charge.wav" );
    if( ( gun == NULL ) || ( charge == NULL )){
        return false;
    }
    
    return true;
}

class Gunner : public myo::DeviceListener {
public:
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        if (pose == myo::Pose::fingersSpread) {
            Mix_PlayChannel( -1, gun, 0 );
            myo->notifyUserAction();

        } else if (pose == myo::Pose::fist){
            Mix_PlayChannel( -1, charge, 0 );
        }
    }
};

int main(int argc, char** argv)
{
    init();
    
    try {
        myo::Hub hub("co.randompaper.myo.gun");
        
        std::cout << "Attempting to find a Myo..." << std::endl;

        myo::Myo* myo = hub.waitForMyo(10000);
        
        if (!myo) {
            throw std::runtime_error("Unable to find a Myo!");
        }
        
        std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
        
        myo->unlock(myo::Myo::unlockHold);
        
        Gunner gunner;
        hub.addListener(&gunner);
        while (1) {
            hub.run(1000/20);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
    }
    
    Mix_CloseAudio();
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
