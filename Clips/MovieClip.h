/*
 ==============================================================================

 Copyright (c) 2019, Foleys Finest Audio - Daniel Walz
 All rights reserved.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.

 ==============================================================================
 */

#pragma once


namespace foleys
{

/**
 @class MovieClip

 The MovieClip plays back a video file. It buffers an amount of audio and
 video frames ahead of time. To check, you can call isFrameAvailable(),
 in case you need to wait for the frame, e.g. for offline bouncing.
 */
class MovieClip   : public AVClip,
                    private juce::AsyncUpdater
{
public:
    MovieClip (VideoEngine& videoEngine);
    virtual ~MovieClip() = default;

    juce::String getDescription() const override;

    bool openFromFile (const juce::File file);

    juce::File getMediaFile() const override;

    void setReader (std::unique_ptr<AVReader> reader);
    void setThumbnailReader (std::unique_ptr<AVReader> reader);

    Size getVideoSize() const override;

    double getLengthInSeconds() const override;

    double getCurrentTimeInSeconds() const override;

    std::pair<int64_t, juce::Image> getFrame (double pts) const override;
    bool isFrameAvailable (double pts) const override;

    juce::Image getCurrentFrame() const override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo&) override;
    void setNextReadPosition (juce::int64 samples) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping (bool shouldLoop) override;

    juce::Image getStillImage (double seconds, Size size) override;

    juce::TimeSliceClient* getBackgroundJob() override;

    bool hasVideo() const override;
    bool hasAudio() const override;

    std::shared_ptr<AVClip> createCopy() override;

    double getSampleRate() const override;

private:

    void handleAsyncUpdate() override;

    class BackgroundReaderJob : public juce::TimeSliceClient
    {
    public:
        BackgroundReaderJob (MovieClip& owner);
        virtual ~BackgroundReaderJob() = default;

        int useTimeSlice() override;

        void setSuspended (bool s);
        bool isSuspended() const;
    private:
        MovieClip& owner;
        bool suspended = true;
        bool inDecodeBlock = false;
    };

    BackgroundReaderJob backgroundJob {*this};
    friend BackgroundReaderJob;

    std::unique_ptr<AVReader> movieReader;
    std::unique_ptr<AVReader> thumbnailReader;
    std::vector<juce::LagrangeInterpolator> resamplers;

    double  sampleRate = {};
    int64_t nextReadPosition = 0;
    int64_t lastShownFrame = -1;
    bool    loop = false;

    Size originalSize;

    VideoFifo videoFifo;
    AudioFifo audioFifo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MovieClip)
};


} // foleys