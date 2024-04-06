find . -regex '.*\.\(cpp\|hpp\|inc\|c\|h\)' -exec clang-format -style=Webkit -i {} \;
