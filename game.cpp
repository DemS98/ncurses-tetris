#include "view/grid.h"
#include "engine/bag.h"
#include "engine/matrix.h"
#include <chrono>
#include <unistd.h>

#ifndef NO_SOUND

#include <opusfile.h>
#include <alsa/asoundlib.h>
#include <thread>
#include <atomic>


// first opus byte
extern const unsigned char _binary_tetris_theme_opus_start[];
// last opus byte
extern const unsigned char _binary_tetris_theme_opus_end[];

void play_sound(std::atomic<bool>& stop) {
    // size of the opus file
    size_t tetris_opus_size = (size_t)(_binary_tetris_theme_opus_end - _binary_tetris_theme_opus_start);

    // error flag
    int err;

    if (stop) {
        return;
    }

    // open opus file
    if(OggOpusFile *oggFile = op_open_memory(_binary_tetris_theme_opus_start, tetris_opus_size, &err)) {
        
        // alsa playback device handler
        snd_pcm_t *handle;

        // alsa frame size
        snd_pcm_sframes_t frames;

        // number of opus file channels
        auto channels = op_head(oggFile, -1)->channel_count;

        // if we can reset the track
        auto seekable = op_seekable(oggFile);

        // open playback device
        if (err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
            // if device cannot be opened
            // clear opus stream and exit
            op_free(oggFile);
            return;
        }

        // set playback device parameters
        if ((err = snd_pcm_set_params(handle,
            SND_PCM_FORMAT_S16,
            SND_PCM_ACCESS_RW_INTERLEAVED,
            channels,
            48000,
            1,
            100000)) < 0) {   /* 0.1sec */

            // if error, clear opus stream and exit
            op_free(oggFile);
            return;
        }

        // 120ms + 48KHz rate * number of channels
        const int oggBufSize = 120 * 48 * channels;
        // allocate ogg -> pcm decoding buffer on the stack
        // each cell is 16 bit signed (2 bytes)
        opus_int16 oggBuf[oggBufSize];

        // while ogg stream has not ended
        // read raw pcm bytes from ogg file
        // samplesPerChannel are the total number of samples (1 sample is 2 bytes signed) per channel read
        while (!stop) {

            // samples per channel, read from decoding ogg to pcm
            int samplesPerChannel = op_read(oggFile, oggBuf, oggBufSize, NULL);

            if (samplesPerChannel <= 0) {
                if (seekable) {
                    op_raw_seek(oggFile, 0);
                } else {
                    // clear ogg stream
                    op_free(oggFile);
                    oggFile = op_open_memory(_binary_tetris_theme_opus_start, tetris_opus_size, &err);
                }
            }

            // wait for playback device to be free
            err = snd_pcm_wait(handle, 1000);
            if (err < 0) {
                // if timeout, exit with error
                break;
            }

            // write decoded pcm bytes to playback
            // we pass samplePerChannel as the number of PCM frames (2 bytes) to be written
            // because we setup alsa playback with n channels, samplePerChannel * n frames are written by alsa
            frames = snd_pcm_writei(handle, oggBuf, samplesPerChannel);

            // if error in writing
            if (frames < 0) {
                // try recovering the device
                err = snd_pcm_recover(handle, frames, 0);
                if (err < 0) {
                    // if unsuccessful, exit with error
                    break;
                }
            }

            // short write info
            if (frames > 0 && frames < (long) samplesPerChannel) {
            }
        }

        // clear ogg stream
        op_free(oggFile);
        // close playback device
        snd_pcm_close(handle);
    }
}

int playback_volume(long volume)
{
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    if (volume == -1) {
        snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_CENTER, &volume);
    } else {
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
    }

    snd_mixer_close(handle);
    return volume;
}

#endif


void drawUI(tetris::view::Grid& grid, unsigned int score) {
    grid.drawMatrix();
    grid.drawScore(score);
    grid.drawBag();    
}

void drawBagTetramines(tetris::view::Grid& grid, const std::vector<tetris::Tetramine *>& tetramines) {
    grid.clearBag();
    grid.drawTetramine(*tetramines.front(), tetris::view::Window::NEXT);
    for(auto i=1; i<tetramines.size(); i++) {
        grid.drawTetramine(*tetramines[i], tetris::view::Window::BAG); 
    }
}

void drawMatrixTetramines(tetris::view::Grid& grid, const std::vector<tetris::Tetramine *>& tetramines) {
    grid.drawMatrix();
    for(const auto tetramine : tetramines) {
        grid.drawTetramine(*tetramine, tetris::view::Window::MATRIX);
    }
}

int main() {
    std::vector<tetris::Tetramine *> tetramines;
    unsigned int score = 0;
    long time = 0;
    tetris::view::Grid grid;
    tetris::Matrix matrix;

    #ifndef NO_SOUND

    int volume = playback_volume(-1);

    std::atomic<bool> stop(false);
    
    std::thread music([&]() {
        play_sound(stop);
    });

    #endif

    sleep(1);

    drawUI(grid, score);

    while (true) {
        
        std::vector<tetris::Tetramine *> spawned {new tetris::Tetramine(COLOR_BLUE), new tetris::Tetramine(COLOR_CYAN),
            new tetris::Tetramine(COLOR_GREEN), new tetris::Tetramine(COLOR_MAGENTA), 
            new tetris::Tetramine(COLOR_YELLOW), new tetris::Tetramine(COLOR_WHITE), new tetris::Tetramine(COLOR_RED)};

        tetris::Bag bag(spawned);
        
        while (!bag.empty()) {
            auto tetramine = bag.pop();
            tetramines.push_back(tetramine);

            drawBagTetramines(grid, bag.getTetramines());

            timeout(10);

            int collision = 0;

            auto moveTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            
            while (collision != 2) {

                auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                
                grid.drawTime(time);

                int ch = getch();
                
                switch (ch) {
                    case KEY_UP:
                        if (!(collision = matrix.collision(tetramine, tetris::Movement::ROTATION))) {
                            grid.clearMatrixTetramine(*tetramine);
                            tetramine->move(tetris::Movement::ROTATION);
                        }
                        break;
                    case KEY_RIGHT:
                        if (!(collision = matrix.collision(tetramine, tetris::Movement::RIGHT))) {
                            grid.clearMatrixTetramine(*tetramine);
                            tetramine->move(tetris::Movement::RIGHT);
                        }
                        break;
                    case KEY_DOWN:
                        if (!(collision = matrix.collision(tetramine, tetris::Movement::DOWN))) {
                            grid.clearMatrixTetramine(*tetramine);
                            tetramine->move(tetris::Movement::DOWN);
                        }
                        break;
                    case KEY_LEFT:
                        if (!(collision = matrix.collision(tetramine, tetris::Movement::LEFT))) {
                            grid.clearMatrixTetramine(*tetramine);
                            tetramine->move(tetris::Movement::LEFT);
                        }
                        break;
                    
                    #ifndef NO_SOUND
                    
                    case '+':
                        volume = playback_volume(volume + 10 < 100 ? volume + 10 : 100);
                        break;
                    case '-':
                        volume = playback_volume(volume - 10 > 0 ? volume - 10 : 0);
                        break;
                    
                    #endif
                    
                    case ERR:
                        int moveTimePassed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - moveTime;
                    
                        if (moveTimePassed >= 150 && collision != 2 && !(collision = matrix.collision(tetramine, tetris::Movement::DOWN))) {
                            grid.clearMatrixTetramine(*tetramine);
                            tetramine->move(tetris::Movement::DOWN);

                            moveTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        }
                        break;
                }

                drawUI(grid, score);
                grid.drawTetramine(*tetramine, tetris::view::Window::MATRIX);

                int timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timestamp;
                
                int timeToSleep = 10 - (timePassed - 10);
                
                timeout(timeToSleep > 0 ? timeToSleep : 0);

                if (tetramine->insideBounds()) {
                    time += timePassed;
                }
            }

            if (!matrix.addTetramine(tetramine)) {
                for(auto tetramine : bag.getTetramines()) {
                    delete tetramine;
                }
                for(auto tetramine : tetramines) {
                    delete tetramine;
                }

                #ifndef NO_SOUND

                stop = true;

                music.join();

                #endif

                timeout(-1);
                
                int ch;
                do
                {
                    ch = getch();
                } while (ch != '\n');

                return 0;
            }

            while (matrix.collapse()) {
                usleep(350 * 1000);
                grid.clearMatrix();
                drawMatrixTetramines(grid, tetramines);
                grid.drawScore(score, score + 100);
                score += 100;
            }

            collision = 0;
        }
    }


}
