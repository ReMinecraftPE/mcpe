import os
import zipfile
import shutil

def extract_minecraft_jar(jar_path, destination):
    """
    Extract specific files and folders from a Minecraft JAR file.

    Args:
        jar_path (str): Path to the minecraft.jar file.
        destination (str): Destination directory to copy the extracted files.
    """
    # Ensure the JAR file exists
    if not os.path.isfile(jar_path):
        print(f"Error: The file {jar_path} does not exist.")
        return

    # Ensure the destination directory exists
    if not os.path.exists(destination):
        os.makedirs(destination)

    # List of files and folders to extract
    items_to_extract = [
        "armor/",
        "art/",
        "environment/",
        "font/",
        "gui/",
        "item/",
        "misc/",
        "mob/",
        "terrain/",
        "title/",
        "pack.png",
        "particles.png",
        "terrain.png",
    ]

    # Open the JAR file as a ZIP archive
    with zipfile.ZipFile(jar_path, 'r') as jar:
        for item in items_to_extract:
            found = False
            for file in jar.namelist():
                # Check if the file starts with the folder name or matches the file
                if file.startswith(item):
                    found = True
                    # Extract the file to the destination directory
                    print(f"Extracting {file}...")
                    jar.extract(file, destination)
            if not found:
                print(f"Warning: {item} not found in {jar_path}.")

    print("Extraction complete.")

if __name__ == "__main__":
    # Path to the minecraft.jar file
    jar_path = input("Enter the path to minecraft.jar: ").strip()

    # Destination directory within the project
    destination = os.path.join("..", "game", "assets", "resource_packs", "minecraft")

    # Extract the JAR file
    extract_minecraft_jar(jar_path, destination)