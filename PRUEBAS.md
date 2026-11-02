# Pruebas

| ID | Requisito | Orden o secuencia | Resultado esperado | Resultado obtenido | OK |
|---|---|---|---|---|---|
| P01 | R1 | `agregar zsh 5.9 7 1` (dos veces) | La segunda vez: `E: No se pudo agregar zsh` |  |  |
| P02 | R2 | `instalar <app> 5` y luego `instalar <otra> 3` | `E: El segundo debe ser un entero mayor o igual que 5` |  |  |
| P03 | R3 |  |  |  |  |
| P04 | R4 |  | `historial`, `deshacer`, `espacio` | Libera el bloque del tope con delete, devuelve sus MB y la app pasa a disponible |  |
| P05 | R5 |  |  |  |  |
| P06 | R6 | secuencia oficial hasta `guardar`, `salir`, abrir, `cargar`, `historial` | Guarda catalogo y pila en tuxstore.txt; al cargar recupera el catalogo, el disco y el espacio libre sin voltear el tope | `Guardado: 8 apps y 5 bloques en tuxstore.txt` / `Cargado: 8 apps y 5 bloques \| libres 57 MB \| turno nuevo` / historial: `vlc, fail2ban, neovim, krita, openssh` | SI |
| P07 | R7 | `hola`, `instalar`, `procesar` con la fila vacia | Mensajes de error; el programa sigue |  |  |

Agreguen todas las que necesiten. Incluyan casos con datos invalidos.
