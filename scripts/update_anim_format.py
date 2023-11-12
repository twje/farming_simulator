import yaml

def read_yaml_file(filepath):
    with open(filepath,'r') as f:
        output = yaml.safe_load(f)
    return output

def write_yaml_file(data, filepath) :
    with open(filepath, 'w',) as f :
        yaml.dump_all(data, f, sort_keys=False)

old_format = read_yaml_file("../graphics/animations/character_new.yaml")
print(old_format)

#new_format = []

# for old_sequence in old_format:
#     new_sequence = {
#         "class": "TextureAnimationSequence",
#         "state": {
#             "sequenceId": old_sequence["sequenceId"],
#             "framesPerSecond": old_sequence["framesPerSecond"],
#             "textures": old_sequence["frames"]["TextureAnimationSequence"]["textures"],
#         }
#     }
#     new_format.append(new_sequence)

# write_yaml_file(new_format, "../graphics/animations/character_new.yaml")