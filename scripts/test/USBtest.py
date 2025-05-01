import os
from sys import platform


def find_usb_drive(drive_name):
    system = platform.system()
    if system == "Windows":
        return find_usb_drive_windows(drive_name)
    elif system == "Linux":
        return find_usb_drive_linux(drive_name)
    else:
        raise NotImplementedError(f"Unsupported operating system: {system}")


def find_usb_drive_windows(drive_name):
    import win32api

    drives = win32api.GetLogicalDriveStrings().split("\000")[:-1]
    for drive in drives:
        if win32api.GetVolumeInformation(drive)[0] == drive_name:
            return drive
    return None


def find_usb_drive_linux(drive_name):
    media_path = "/media"
    users = os.listdir(media_path)
    for user in users:
        user_path = os.path.join(media_path, user)
        if os.path.isdir(user_path):
            drives = os.listdir(user_path)
            for drive in drives:
                if drive == drive_name:
                    return os.path.join(user_path, drive)
    return None


if __name__ == "__main__":
    drivepath = find_usb_drive("VOID")
    if not drivepath:
        print("USB test failed: USB drive not found")
        exit(1)

    file = os.open(os.path.join(drivepath, "test.txt"), os.O_RDONLY)
    text = os.read(file, 100)
    os.close(file)

    if text.find("Hello World") != -1:
        print("USB test passed")
        exit(0)
    else:
        print("USB test failed: File content mismatch")
        exit(1)
