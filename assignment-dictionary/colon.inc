%define address 0
%macro colon 2
%2:
    %ifdef address
      dq address
    %endif
    db %1, 0
    %define address %2
%endmacro
