package eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty;

import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.ThirdPartyDoor;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.ThirdPartyDoor.DoorState;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.exceptions.CannotChangeCodeForUnlockedDoor;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.exceptions.CannotChangeStateOfLockedDoor;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.exceptions.CannotUnlockDoorException;
import eu.jpereira.trainings.designpatterns.structural.adapter.exceptions.CodeMismatchException;
import eu.jpereira.trainings.designpatterns.structural.adapter.exceptions.IncorrectDoorCodeException;
import eu.jpereira.trainings.designpatterns.structural.adapter.model.Door;

public class ThirdPartyDoorAdapter extends ThirdPartyDoor implements Door{

    @Override 
    public boolean testCode(String code) {
        try{
            unlock(code);
            lock();
            return true;
        } catch (CannotUnlockDoorException e) {
            return false;
        }
    }

    @Override
        public void changeCode(String oldCode, String newCode, String newCodeConfirmation)
            throws IncorrectDoorCodeException, CodeMismatchException {

        if (!newCode.equals(newCodeConfirmation)) {
            throw new CodeMismatchException();
        }

        try {
            unlock(oldCode);
            setNewLockCode(newCode);
            lock();
        } catch (CannotUnlockDoorException ex) {
            throw new IncorrectDoorCodeException(ex);
        } catch (CannotChangeCodeForUnlockedDoor e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public boolean isOpen() {
        return getState() == DoorState.OPEN;
    }

    @Override
    public void close() {
    try {
            setState(DoorState.CLOSED);
            lock();
        } catch (CannotChangeStateOfLockedDoor e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public void open(String code) throws IncorrectDoorCodeException {
        try {
            unlock(code);
            setState(DoorState.OPEN);
        } catch (CannotUnlockDoorException e) {
            throw new IncorrectDoorCodeException(e);
        } catch (CannotChangeStateOfLockedDoor e) {
            throw new RuntimeException(e);
        }
    }

}   