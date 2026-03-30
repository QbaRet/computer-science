package eu.jpereira.trainings.designpatterns.structural.decorator.channel.decorator;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannel;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannelBuilder;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannelProperties;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannelPropertyKey;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.spy.TestSpySocialChannel;

public class ChainCensorDecoratorTest extends AbstractSocialChanneldDecoratorTest {

    @Test
    public void testChainCensorWithOtherDecorators() {
        // Create the builder
        SocialChannelBuilder builder = createTestSpySocialChannelBuilder();

        // create a spy social channel
        SocialChannelProperties props = new SocialChannelProperties().putProperty(SocialChannelPropertyKey.NAME, TestSpySocialChannel.NAME);
        
        String wordToCensor = "damn";
        String urlToAppend = "http://jpereira.eu";
        int truncateLength = 20; // Example length, adjust as needed

        SocialChannel channel = builder
                .with(new URLAppender(urlToAppend))
                .with(new WordCensor(wordToCensor))
                .with(new MessageTruncator(truncateLength))
                .getDecoratedChannel(props);

        // Now call channel.deliverMessage
        String originalMessage = "this is a " + wordToCensor + " message with a very long text that should be truncated";
        channel.deliverMessage(originalMessage);

        // Spy channel. Should get the one created before
        TestSpySocialChannel spyChannel = (TestSpySocialChannel) builder.buildChannel(props);

        // Build expected masked word
        StringBuilder mask = new StringBuilder();
        for (int i = 0; i < wordToCensor.length(); i++) {
            mask.append('*');
        }

        // Expected message after censoring, appending URL, and truncating
        // Expected message after censoring, appending URL, and truncating
        // With the current chain: URLAppender -> WordCensor -> MessageTruncator(20)
        // 1. Original message: "this is a damn message with a very long text that should be truncated"
        // 2. After URLAppender: "this is a damn message with a very long text that should be truncated http://jpereira.eu"
        String messageAfterUrl = originalMessage + " " + urlToAppend;

        // 3. After WordCensor: censors "damn" to "****"
        String messageAfterCensor = messageAfterUrl.replace(wordToCensor, mask.toString());

        // 4. After MessageTruncator (last in chain): truncates to 20 chars
        String expectedFinalMessage;
        if (messageAfterCensor.length() > truncateLength) {
            expectedFinalMessage = messageAfterCensor.substring(0, truncateLength - 3) + "...";
        } else {
            expectedFinalMessage = messageAfterCensor;
        }

        assertEquals(expectedFinalMessage, spyChannel.lastMessagePublished());
    }
}