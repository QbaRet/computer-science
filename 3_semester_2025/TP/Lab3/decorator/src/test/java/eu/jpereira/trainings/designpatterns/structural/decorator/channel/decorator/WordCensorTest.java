package eu.jpereira.trainings.designpatterns.structural.decorator.channel.decorator;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannel;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannelBuilder;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannelProperties;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannelPropertyKey;
import eu.jpereira.trainings.designpatterns.structural.decorator.channel.spy.TestSpySocialChannel;

/**
 * @author jpereira
 * 
 */
public class WordCensorTest extends AbstractSocialChanneldDecoratorTest{
	@Test
	public void testCensor() {
		SocialChannelBuilder builder = createTestSpySocialChannelBuilder();
		SocialChannelProperties props = new SocialChannelProperties().putProperty(SocialChannelPropertyKey.NAME, TestSpySocialChannel.NAME);
		String wordToCensor = "damn";
		SocialChannel channel = builder.with(new WordCensor(wordToCensor)).getDecoratedChannel(props);

		channel.deliverMessage("this is a " + wordToCensor + " message");
		TestSpySocialChannel spyChannel = (TestSpySocialChannel) builder.buildChannel(props);
		StringBuilder mask = new StringBuilder();
		for (int i = 0; i < wordToCensor.length(); i++) {
			mask.append('*');
		}

		assertEquals("this is a " + mask.toString() + " message", spyChannel.lastMessagePublished());
	}

	
}
