from pathlib import Path

def extract_last_n_directories_with_filename(directory_count, directory_path):
    directory_path = Path(directory_path)
    
    if directory_path.is_dir():
        for file_path in directory_path.iterdir():
            prefix = ""
            for index in range(directory_count + 1, 1, -1):                
                prefix += f"{file_path.parts[-index]}_"

            resourcePath = str(directory_path).replace("\\", "/") + "/" + file_path.name
            resourceID = f"{prefix}{file_path.stem}"
            print(f"{resourceID}, ../{resourcePath}")
        

def filename(directory_path):
    directory_path = Path(directory_path)
    
    if directory_path.is_dir():    
        for file_path in directory_path.iterdir():
            resourcePath = str(directory_path).replace("\\", "/") + "/" + file_path.name
            resourceID = file_path.stem
            print(f"{resourceID}, ../{resourcePath}")


def reverse_csv(input_filepath, output_filepath):
    with open(input_filepath, "r") as input_file:        
        lines = input_file.readlines()        
    
    with open(output_filepath, "w") as output_file:
        for line in lines:
            resourcePath = line.split(", ")[0].strip()
            resourceID = line.split(", ")[1].strip()            
            output_file.write(f"{resourceID}, ../{resourcePath}\n")


extract_last_n_directories_with_filename(2, R"../graphics/character/down_water")

