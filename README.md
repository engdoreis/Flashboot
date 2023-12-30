# FlashBoot 
Flashboot is a light weight and fast bootloader for embedded applications.

It compiles to a lib that should be linked to an application that is specific to the platform. The application is responsible for providing the hardware abstraction via callbacks.

It features:
 - Internal protocol for:
    - Loading a new binary.
    - Updating an existing binary (Over-the-air update). Where the application receives the new binary, flash it in a flash regions, and request an update to the Bootloader followed by a reset.
- CRC32 verification.
- Rollback.

## Nix environment
By initializing a nix shell it will fetch all the dependencies.
```sh
nix develop
```

## Build
```sh 
make static cpu=x86
```

## Testing
```sh 
make test
```
**Result**
```console
+-----------------------------------------------------------------------------------+
|                                STARTING AUTO TEST                                 |
+-----------------------------------------------------------------------------------+
| Test case 1: Bootloader success                                          Approved |
+-----------------------------------------------------------------------------------+
| Test case 2: Bootloader CRC error                                        Approved |
+-----------------------------------------------------------------------------------+
| Test case 3: Bootloader alingment error                                  Approved |
+-----------------------------------------------------------------------------------+
| Test case 4: Bootloader overflow error                                   Approved |
+-----------------------------------------------------------------------------------+
| Test case 5: Bootloader invalid address                                  Approved |
+-----------------------------------------------------------------------------------+
| Test case 6: Self update success                                         Approved |
+-----------------------------------------------------------------------------------+
| Test case 7: Self update CRC error                                       Approved |
+-----------------------------------------------------------------------------------+
| Test case 8: Backup success                                              Approved |
+-----------------------------------------------------------------------------------+
```