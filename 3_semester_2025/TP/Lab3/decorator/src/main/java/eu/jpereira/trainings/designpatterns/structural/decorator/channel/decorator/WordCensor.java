package eu.jpereira.trainings.designpatterns.structural.decorator.channel.decorator;

import eu.jpereira.trainings.designpatterns.structural.decorator.channel.SocialChannel;

public class WordCensor extends SocialChannelDecorator { 

    private String wordToCensor; 

    public WordCensor(String wordToCensor) { 
        this.wordToCensor = wordToCensor; 
    } 

    public WordCensor(String wordToCensor, SocialChannel decoratedChannel) { 
        this.wordToCensor = wordToCensor; 
        this.delegate = decoratedChannel; 
    } 

    @Override 
    public void deliverMessage(String message) { 
        String censoredWord = generateCensoredWord(wordToCensor.length()); 
        message = message.replace(wordToCensor, censoredWord); 
        delegate.deliverMessage(message); 
    } 

    private String generateCensoredWord(int length) { 
        StringBuilder builder = new StringBuilder(); 
        for (int i = 0; i < length; i++) { 
            builder.append("*"); 
        } 
        return builder.toString(); 
    } 
}