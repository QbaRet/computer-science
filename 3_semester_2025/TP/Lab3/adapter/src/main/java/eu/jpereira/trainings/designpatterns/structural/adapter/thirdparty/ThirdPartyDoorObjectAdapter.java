package eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty;
import eu.jpereira.trainings.designpatterns.structural.adapter.model.Door;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.exceptions.CannotUnlockDoorException;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.ThirdPartyDoor.DoorState;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.exceptions.CannotChangeCodeForUnlockedDoor;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.exceptions.CannotChangeStateOfLockedDoor;
import eu.jpereira.trainings.designpatterns.structural.adapter.exceptions.CodeMismatchException;
import eu.jpereira.trainings.designpatterns.structural.adapter.exceptions.IncorrectDoorCodeException;

public class ThirdPartyDoorObjectAdapter implements Door{
    private ThirdPartyDoor thirdPartyDoor;

    public ThirdPartyDoorObjectAdapter() {
        this.thirdPartyDoor = new ThirdPartyDoor();
    }

    @Override 
    public boolean testCode(String code) {
        try{
            thirdPartyDoor.unlock(code);
            thirdPartyDoor.lock();
            return true;
        } catch (CannotUnlockDoorException e) {
            return false;
        }
    }

    @Override
    public void changeCode(String oldCode, String newCode, String newCodeConfirmation) throws IncorrectDoorCodeException, CodeMismatchException {
        if (!newCode.equals(newCodeConfirmation)) {
            throw new CodeMismatchException();
        }

        try {
            thirdPartyDoor.unlock(oldCode);
            thirdPartyDoor.setNewLockCode(newCode);
            thirdPartyDoor.lock();
        } catch (CannotUnlockDoorException ex) {
            throw new IncorrectDoorCodeException(ex);
        } catch (CannotChangeCodeForUnlockedDoor e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public boolean isOpen() {
        return thirdPartyDoor.getState() == ThirdPartyDoor.DoorState.OPEN;
    }

    @Override
    public void close() {
        try {
            thirdPartyDoor.setState(DoorState.CLOSED);
            thirdPartyDoor.lock();
        } catch (CannotChangeStateOfLockedDoor e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public void open(String code) throws IncorrectDoorCodeException {
        try {
            thirdPartyDoor.unlock(code);
            thirdPartyDoor.setState(DoorState.OPEN);
        } catch (CannotUnlockDoorException e) {
            throw new IncorrectDoorCodeException(e);
        } catch (CannotChangeStateOfLockedDoor e) {
            throw new RuntimeException(e);
        }
    }
}