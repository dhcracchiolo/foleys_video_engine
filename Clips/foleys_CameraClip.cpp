/*
 ==============================================================================

 Copyright (c) 2020, Foleys Finest Audio - Daniel Walz
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

#if FOLEYS_CAMERA_SUPPORT

namespace foleys
{

CameraClip::CameraClip (VideoEngine& videoEngine, std::unique_ptr<CameraReceiver> sourceToUse)
  : AVClip (videoEngine),
    source (std::move (sourceToUse))
{
    if (source.get() != nullptr)
    {
        source->onFrameCaptured =
        [this]{
            auto seconds = sampleRate > 0.0 ? position / sampleRate : 0.0;
            sendTimecode (position, seconds, juce::sendNotification);
        };
    }
}

juce::String CameraClip::getDescription() const
{
    return NEEDS_TRANS ("Not connected");
}

Size CameraClip::getVideoSize() const
{
    return {};
}

double CameraClip::getLengthInSeconds() const
{
    return std::numeric_limits<double>::max();
}

VideoFrame& CameraClip::getFrame (double pts)
{
    juce::ignoreUnused (pts);
    return source->getCurrentFrame();
}

#if FOLEYS_USE_OPENGL
void CameraClip::render (double pts)
{
    juce::ignoreUnused (pts);
}
#endif

bool CameraClip::isFrameAvailable (double pts) const
{
    juce::ignoreUnused (pts);
    return true;
}

juce::Image CameraClip::getStillImage (double seconds, Size size)
{
    juce::ignoreUnused (seconds);
    juce::ignoreUnused (size);

    return {};
}

double CameraClip::getSampleRate() const
{
    return 48000.0;
}

double CameraClip::getCurrentTimeInSeconds() const
{
    return 0.0;
}

void CameraClip::setNextReadPosition (juce::int64 newPosition)
{
    position = newPosition;
}

juce::int64 CameraClip::getNextReadPosition() const
{
    return position;
}

juce::int64 CameraClip::getTotalLength() const
{
    return std::numeric_limits<juce::int64>::max();
}

void CameraClip::prepareToPlay (int samplesPerBlockExpected, double newSampleRate)
{
    juce::ignoreUnused (samplesPerBlockExpected);

    sampleRate = newSampleRate;
}

void CameraClip::releaseResources()
{
}

void CameraClip::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    position += bufferToFill.numSamples;
}

std::shared_ptr<AVClip> CameraClip::createCopy (StreamTypes types)
{
    juce::ignoreUnused (types);
    return {};
}

} // namespace foleys

#endif // FOLEYS_CAMERA_SUPPORT
