#include "Game.h"

void Game::loadMusic() {
	AudioEngine& audioEngine = AudioEngine::GetInstance();

	audioEngine.Init();
	audioEngine.LoadBank("Master");

	audioEngine.LoadEvent("Music");
	audioEngine.PlayEvent("Music");
	//audioEngineManager.push_back(audioEngine);

	//Create your sounds here, will be something like createSoundParam("MonkeySound", 400, {0.0, 1.0, 40.0})
	//To play a sound just turn corresponding bool you want with the sound to 3 somewhere in the update 
}
void Game::updateMusic() {

	AudioEngine::GetInstance().SetEventPosition("Music", { 20, 2, 10 });//cameraPos); 
	//AudioEngine::GetInstance().SetEventPosition("Crash", newPosition); //Tutorial code
	AudioEngine::GetInstance().SetListenerPosition(cameraPos); //Do we also want to add the what you are looking at? Or just the position? Will you even see this and respond?
	AudioEngine::GetInstance().Update();
	int hello = 0;
}
void Game::playSound() {
	if (soundsToPlay.size() > 0) {
		for (int i = 0; i < soundsToPlay.size(); i++) { //Initialise the sound
			if (soundsToPlay[i] == 0) {
				; //Just to save some processing since this will be true most of the time, so one check rather then three and the rare occasion it's +1 check
			}
			else if (soundsToPlay[i] == 3) {
				soundsToPlay[i] = 2;
				AudioEngine::GetInstance().LoadEvent(soundsName[i]);
				AudioEngine::GetInstance().PlayEvent(soundsName[i]);
				AudioEngine::GetInstance().SetEventPosition(soundsName[i], soundPosition[i]);
				timerForSounds[i] = timerForSoundsdefault[i];

			}
			else if (soundsToPlay[i] == 2) { //Play the sound is in the normal update I assume so this just runs the timer
				timerForSounds[i]--;
				if (timerForSounds[i] == 0) {
					soundsToPlay[i] = 1;
				}
			}
			else if (soundsToPlay[i] == 1) {
				AudioEngine::GetInstance().StopEvent(soundsName[i], true);
				soundsToPlay[i] = 0;
			}
		}
	}
}
void Game::shutDownSound() {
	AudioEngine::GetInstance().Shutdown();
}

void Game::createSoundParam(std::string name, int timerLength, glm::vec3 position, int toPlay) {
	soundsToPlay.push_back(toPlay);
	timerForSoundsdefault.push_back(timerLength);
	timerForSounds.push_back(timerLength);
	soundsName.push_back(name);
	soundPosition.push_back(position);
}
