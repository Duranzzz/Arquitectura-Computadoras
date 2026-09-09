Attribute VB_Name = "ModuloSimuladorCPU"
Option Explicit

' ==============================================================================
' SIMULADOR DE COMPUTADOR - ARQUITECTURA DE VON NEUMANN (EXCEL VBA)
' Modelo Funcional: Eduardo Alcalde Lancharro & Miguel García López
' Ciclo de Instrucción: Búsqueda (Fetch), Decodificación y Ejecución
' Autor: Leonardo Durán Cuenca
' ==============================================================================

' Constantes de color para resaltado visual
Public Const COLOR_RESALTADO As Long = 6876159       ' RGB(255, 235, 59) Amarillo vibrante
Public Const COLOR_BLANCO As Long = 16777215         ' RGB(255, 255, 255) Blanco
Public Const COLOR_FLECHA_ACTIVA As Long = 23295     ' RGB(255, 90, 0) Naranja/Rojo activo
Public Const COLOR_FLECHA_NEUTRA As Long = 11184810  ' RGB(170, 170, 170) Gris neutro
Public Const RETARDO_MICROORDEN As Double = 0.4      ' Segundos de pausa visual

' ==============================================================================
' 1. MÁQUINA DE ESTADOS PRINCIPAL: PasoCicloReloj
' ==============================================================================
Public Sub PasoCicloReloj()
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    Desproteger ws
    
    Dim sFase As String
    sFase = Trim(UCase(ws.Range("Fase").Value))
    
    Select Case sFase
        Case "INICIO", ""
            ' ------------------------------------------------------------------
            ' Paso 1: INICIO -> RDM = CP -> Fase = FETCH_RIM
            ' ------------------------------------------------------------------
            ws.Range("RDM").Value = ws.Range("CP").Value
            ws.Range("Fase").Value = "FETCH_RIM"
            ws.Range("Reloj").Value = "T1"
            ws.Range("BusDirecciones").Value = "CP -> RDM (" & ws.Range("CP").Value & ")"
            ws.Range("BusControl").Value = "INACTIVO"
            ws.Range("SelectorMem").Value = "INACTIVO"
            ws.Range("Consola").Value = "Fase Búsqueda [T1]: Transferencia de dirección de la siguiente instrucción (RDM <- CP)."
            
            AnimarFlecha "FlechaCP_RDM"
            ResaltarCambio ws.Range("RDM")
            RegistrarPasoEnTraza "RDM <- CP"
            
        Case "FETCH_RIM"
            ' ------------------------------------------------------------------
            ' Paso 2: FETCH_RIM -> RIM = BuscarEnMemoria(RDM) -> Fase = FETCH_RI
            ' ------------------------------------------------------------------
            Dim dirRDM As String
            dirRDM = Trim(ws.Range("RDM").Value)
            
            ws.Range("SelectorMem").Value = "ACTIVO"
            ws.Range("BusControl").Value = "LECTURA"
            ws.Range("BusDirecciones").Value = "RDM -> Memoria (" & dirRDM & ")"
            AnimarFlecha "FlechaRDM_Mem"
            
            Dim contenido As String
            contenido = BuscarEnMemoria(dirRDM)
            
            ws.Range("RIM").Value = contenido
            ws.Range("BusDatos").Value = "RAM -> RIM (" & contenido & ")"
            ws.Range("Fase").Value = "FETCH_RI"
            ws.Range("Reloj").Value = "T2"
            ws.Range("Consola").Value = "Fase Búsqueda [T2]: Lectura de Memoria Central hacia el Registro de Información (RIM <- Mem[" & dirRDM & "])."
            
            AnimarFlecha "FlechaMem_RIM"
            ResaltarCambio ws.Range("RIM")
            RegistrarPasoEnTraza "RIM <- Mem[RDM]"
            
        Case "FETCH_RI"
            ' ------------------------------------------------------------------
            ' Paso 3: FETCH_RI -> RI = RIM -> Fase = DECODIFICAR
            ' ------------------------------------------------------------------
            ws.Range("RI").Value = ws.Range("RIM").Value
            ws.Range("BusDatos").Value = "RIM -> RI (" & ws.Range("RIM").Value & ")"
            ws.Range("Fase").Value = "DECODIFICAR"
            ws.Range("Reloj").Value = "T3"
            ws.Range("Consola").Value = "Fase Búsqueda [T3]: Carga de la instrucción en el Registro de Instrucción (RI <- RIM)."
            
            AnimarFlecha "FlechaRIM_RI"
            ResaltarCambio ws.Range("RI")
            RegistrarPasoEnTraza "RI <- RIM"
            
        Case "DECODIFICAR"
            ' ------------------------------------------------------------------
            ' Paso 4: DECODIFICAR -> separar RI en CO, OP1, OP2, OP3 -> Fase = INC_CP
            ' ------------------------------------------------------------------
            DecodificarInstruccion
            ws.Range("Fase").Value = "INC_CP"
            ws.Range("Reloj").Value = "T4"
            ws.Range("Consola").Value = "Fase Decodificación [T4]: Decodificador extrae CO=" & ws.Range("CO").Value & " y direcciones de operandos."
            
            ResaltarCambio ws.Range("C7:C10")
            RegistrarPasoEnTraza "Decodificar RI (CO, OP1, OP2, OP3)"
            
        Case "INC_CP"
            ' ------------------------------------------------------------------
            ' Paso 5: INC_CP -> CP = CP + 1 -> Fase = EJECUTAR
            ' ------------------------------------------------------------------
            Dim valCP As Long
            valCP = Val(ws.Range("CP").Value) + 1
            ws.Range("CP").Value = Format(valCP, "000")
            ws.Range("Fase").Value = "EJECUTAR"
            ws.Range("Reloj").Value = "T5"
            ws.Range("Consola").Value = "Fase Búsqueda [T5]: Incremento del Contador de Programa (CP <- CP + 1 = " & ws.Range("CP").Value & ")."
            
            ResaltarCambio ws.Range("CP")
            RegistrarPasoEnTraza "CP <- CP + 1"
            
        Case "EJECUTAR"
            ' ------------------------------------------------------------------
            ' Paso 6: EJECUTAR -> Select Case CO -> Fase = INICIO (o FIN si HLT)
            ' ------------------------------------------------------------------
            EjecutarInstruccionActual
            
        Case "FIN"
            ' ------------------------------------------------------------------
            ' Paso 7: FIN -> CPU detenida, requiere ResetCPU
            ' ------------------------------------------------------------------
            ws.Range("Consola").Value = "La CPU está DETENIDA (HLT alcanzado). Presione 'ResetCPU' para reiniciar el simulador."
            MsgBox "El programa ha finalizado exitosamente (instrucción HLT alcanzada)." & vbCrLf & _
                   "Presione el botón 'ResetCPU' para reiniciar la simulación.", vbInformation, "Simulador CPU - Programa Finalizado"
                   
        Case Else
            ws.Range("Consola").Value = "Estado desconocido: '" & sFase & "'. Reiniciando a INICIO."
            ws.Range("Fase").Value = "INICIO"
    End Select
    Reproteger ws
End Sub

' ==============================================================================
' 2. EJECUCIÓN DEL CONJUNTO DE INSTRUCCIONES
' ==============================================================================
Private Sub EjecutarInstruccionActual()
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    
    Dim sCO As String
    sCO = Trim(UCase(ws.Range("CO").Value))
    
    Dim sOP1 As String, sOP2 As String, sOP3 As String
    sOP1 = Trim(ws.Range("_OP1").Value)
    sOP2 = Trim(ws.Range("_OP2").Value)
    sOP3 = Trim(ws.Range("_OP3").Value)
    
    Dim val1 As Double, val2 As Double, res As Double
    Dim vMem1 As String, vMem2 As String
    
    Select Case sCO
        Case "SUMAR"
            ' Mem[OP3] = Mem[OP1] + Mem[OP2]
            ws.Range("Reloj").Value = "T6"
            ws.Range("Consola").Value = "Fase Ejecución: Operación SUMAR. Leyendo operandos desde Mem[" & sOP1 & "] y Mem[" & sOP2 & "]."
            
            ' 1. Cargar Operando 1 en REN1
            ws.Range("RDM").Value = sOP1
            ws.Range("SelectorMem").Value = "ACTIVO"
            ws.Range("BusControl").Value = "LECTURA"
            ws.Range("BusDirecciones").Value = "OP1 -> RDM (" & sOP1 & ")"
            AnimarFlecha "FlechaRDM_Mem"
            vMem1 = BuscarEnMemoria(sOP1)
            ws.Range("RIM").Value = vMem1
            AnimarFlecha "FlechaMem_RIM"
            ws.Range("_REN1").Value = vMem1
            AnimarFlecha "FlechaMem_REN"
            ResaltarCambio ws.Range("_REN1")
            
            ' 2. Cargar Operando 2 en REN2
            ws.Range("RDM").Value = sOP2
            ws.Range("BusDirecciones").Value = "OP2 -> RDM (" & sOP2 & ")"
            AnimarFlecha "FlechaRDM_Mem"
            vMem2 = BuscarEnMemoria(sOP2)
            ws.Range("RIM").Value = vMem2
            AnimarFlecha "FlechaMem_RIM"
            ws.Range("_REN2").Value = vMem2
            AnimarFlecha "FlechaMem_REN"
            ResaltarCambio ws.Range("_REN2")
            
            ' 3. Cálculo en UAL y Acumulador
            ws.Range("COP").Value = "SUMA (+)"
            val1 = Val(ws.Range("_REN1").Value)
            val2 = Val(ws.Range("_REN2").Value)
            res = val1 + val2
            ws.Range("Ac").Value = CStr(res)
            AnimarFlecha "FlechaALU_Ac"
            ResaltarCambio ws.Range("Ac")
            ActualizarBanderas res
            
            ' 4. Almacenar Resultado en OP3
            ws.Range("RDM").Value = sOP3
            ws.Range("RIM").Value = CStr(res)
            ws.Range("BusControl").Value = "ESCRITURA"
            ws.Range("BusDirecciones").Value = "OP3 -> RDM (" & sOP3 & ")"
            ws.Range("BusDatos").Value = "RIM -> RAM (" & CStr(res) & ")"
            EscribirEnMemoria sOP3, CStr(res)
            AnimarFlecha "FlechaRDM_Mem"
            
            ws.Range("Consola").Value = "Ejecución SUMAR completada: Mem[" & sOP3 & "] = " & val1 & " + " & val2 & " = " & res & "."
            RegistrarPasoEnTraza "Mem[" & sOP3 & "] <- REN1 + REN2 (" & res & ")"
            ws.Range("Fase").Value = "INICIO"
            
        Case "RESTAR"
            ' Mem[OP3] = Mem[OP1] - Mem[OP2]
            ws.Range("Reloj").Value = "T6"
            ws.Range("Consola").Value = "Fase Ejecución: Operación RESTAR. Leyendo operandos desde Mem[" & sOP1 & "] y Mem[" & sOP2 & "]."
            
            ' Cargar Operando 1
            ws.Range("RDM").Value = sOP1
            ws.Range("SelectorMem").Value = "ACTIVO"
            ws.Range("BusControl").Value = "LECTURA"
            AnimarFlecha "FlechaRDM_Mem"
            vMem1 = BuscarEnMemoria(sOP1)
            ws.Range("RIM").Value = vMem1
            ws.Range("_REN1").Value = vMem1
            AnimarFlecha "FlechaMem_REN"
            ResaltarCambio ws.Range("_REN1")
            
            ' Cargar Operando 2
            ws.Range("RDM").Value = sOP2
            AnimarFlecha "FlechaRDM_Mem"
            vMem2 = BuscarEnMemoria(sOP2)
            ws.Range("RIM").Value = vMem2
            ws.Range("_REN2").Value = vMem2
            AnimarFlecha "FlechaMem_REN"
            ResaltarCambio ws.Range("_REN2")
            
            ' UAL
            ws.Range("COP").Value = "RESTA (-)"
            val1 = Val(ws.Range("_REN1").Value)
            val2 = Val(ws.Range("_REN2").Value)
            res = val1 - val2
            ws.Range("Ac").Value = CStr(res)
            AnimarFlecha "FlechaALU_Ac"
            ResaltarCambio ws.Range("Ac")
            ActualizarBanderas res
            
            ' Almacenar en OP3
            ws.Range("RDM").Value = sOP3
            ws.Range("RIM").Value = CStr(res)
            ws.Range("BusControl").Value = "ESCRITURA"
            EscribirEnMemoria sOP3, CStr(res)
            AnimarFlecha "FlechaRDM_Mem"
            
            ws.Range("Consola").Value = "Ejecución RESTAR completada: Mem[" & sOP3 & "] = " & val1 & " - " & val2 & " = " & res & "."
            RegistrarPasoEnTraza "Mem[" & sOP3 & "] <- REN1 - REN2 (" & res & ")"
            ws.Range("Fase").Value = "INICIO"
            
        Case "CARGAR"
            ' Ac = Mem[OP1]
            ws.Range("Reloj").Value = "T6"
            ws.Range("RDM").Value = sOP1
            ws.Range("SelectorMem").Value = "ACTIVO"
            ws.Range("BusControl").Value = "LECTURA"
            AnimarFlecha "FlechaRDM_Mem"
            vMem1 = BuscarEnMemoria(sOP1)
            ws.Range("RIM").Value = vMem1
            ws.Range("Ac").Value = vMem1
            AnimarFlecha "FlechaALU_Ac"
            ResaltarCambio ws.Range("Ac")
            ActualizarBanderas Val(vMem1)
            
            ws.Range("Consola").Value = "Ejecución CARGAR: Ac <- Mem[" & sOP1 & "] (" & vMem1 & ")."
            RegistrarPasoEnTraza "Ac <- Mem[" & sOP1 & "]"
            ws.Range("Fase").Value = "INICIO"
            
        Case "ALMACENAR"
            ' Mem[OP1] = Ac
            ws.Range("Reloj").Value = "T6"
            ws.Range("RDM").Value = sOP1
            ws.Range("RIM").Value = ws.Range("Ac").Value
            ws.Range("SelectorMem").Value = "ACTIVO"
            ws.Range("BusControl").Value = "ESCRITURA"
            AnimarFlecha "FlechaRDM_Mem"
            EscribirEnMemoria sOP1, ws.Range("Ac").Value
            
            ws.Range("Consola").Value = "Ejecución ALMACENAR: Mem[" & sOP1 & "] <- Ac (" & ws.Range("Ac").Value & ")."
            RegistrarPasoEnTraza "Mem[" & sOP1 & "] <- Ac"
            ws.Range("Fase").Value = "INICIO"
            
        Case "SALTAR"
            ' CP = OP1
            ws.Range("Reloj").Value = "T6"
            ws.Range("CP").Value = Format(Val(sOP1), "000")
            ResaltarCambio ws.Range("CP")
            ws.Range("Consola").Value = "Ejecución SALTAR: Salto incondicional a dirección " & ws.Range("CP").Value & "."
            RegistrarPasoEnTraza "CP <- " & sOP1
            ws.Range("Fase").Value = "INICIO"
            
        Case "SALTAR_SI_CERO"
            ' if FlagZ = 1, CP = OP1
            ws.Range("Reloj").Value = "T6"
            If Val(ws.Range("FlagZ").Value) = 1 Then
                ws.Range("CP").Value = Format(Val(sOP1), "000")
                ResaltarCambio ws.Range("CP")
                ws.Range("Consola").Value = "Ejecución SALTAR_SI_CERO: Condición Z=1 cumplida. Salto a dirección " & ws.Range("CP").Value & "."
                RegistrarPasoEnTraza "CP <- " & sOP1 & " (Z=1)"
            Else
                ws.Range("Consola").Value = "Ejecución SALTAR_SI_CERO: Condición Z=0 no cumplida. Continúa flujo secuencial."
                RegistrarPasoEnTraza "Salto no ejecutado (Z=0)"
            End If
            ws.Range("Fase").Value = "INICIO"
            
        Case "HLT"
            ws.Range("Reloj").Value = "T6"
            ws.Range("Fase").Value = "FIN"
            ws.Range("Consola").Value = "Instrucción HLT: Detención del computador. Fin del programa."
            RegistrarPasoEnTraza "HLT (Detener CPU)"
            MsgBox "Programa terminado exitosamente (instrucción HLT alcanzada).", vbInformation, "Simulador CPU"
            
        Case Else
            ws.Range("Consola").Value = "Instrucción no reconocida: '" & sCO & "'."
            RegistrarPasoEnTraza "ERROR: Opcode desconocido (" & sCO & ")"
            ws.Range("Fase").Value = "INICIO"
    End Select
End Sub

' ==============================================================================
' 3. MACRO: EjecutarInstruccionCompleta (Una instrucción completa por clic)
' ==============================================================================
Public Sub EjecutarInstruccionCompleta()
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    
    If Trim(UCase(ws.Range("Fase").Value)) = "FIN" Then
        MsgBox "El programa ya ha alcanzado el estado FIN (HLT)." & vbCrLf & _
               "Presione 'ResetCPU' para reiniciar antes de ejecutar nuevamente.", vbExclamation, "CPU Detenida"
        Exit Sub
    End If
    
    Dim contadorSeguridad As Integer
    contadorSeguridad = 0
    
    Do
        PasoCicloReloj
        contadorSeguridad = contadorSeguridad + 1
        
        Dim faseActual As String
        faseActual = Trim(UCase(ws.Range("Fase").Value))
        
        If faseActual = "INICIO" Or faseActual = "FIN" Then
            Exit Do
        End If
        
        If contadorSeguridad > 25 Then
            MsgBox "Límite de seguridad alcanzado (25 microórdenes sin retorno a INICIO/FIN).", vbExclamation, "Seguridad"
            Exit Do
        End If
        
        PausaSegundos 0.2
    Loop
End Sub

' ==============================================================================
' 4. MACRO: ResetCPU
' ==============================================================================
Public Sub ResetCPU()
    Dim wsCPU As Worksheet, wsTraza As Worksheet
    Set wsCPU = ThisWorkbook.Sheets("Simulador_CPU")
    Set wsTraza = ThisWorkbook.Sheets("Tabla_de_Traza")
    Desproteger wsCPU
    
    ' 1. Restaurar Registros de Unidad de Control
    wsCPU.Range("CP").Value = "006"
    wsCPU.Range("RI").Value = "SUMAR 033 992 993"
    wsCPU.Range("CO").Value = "SUMAR"
    wsCPU.Range("_OP1").Value = "033"
    wsCPU.Range("_OP2").Value = "992"
    wsCPU.Range("_OP3").Value = "993"
    wsCPU.Range("Fase").Value = "INICIO"
    
    ' 2. Restaurar Registros de UAL
    wsCPU.Range("_REN1").Value = "0"
    wsCPU.Range("_REN2").Value = "0"
    wsCPU.Range("COP").Value = "SUMA (+)"
    wsCPU.Range("Ac").Value = "0"
    wsCPU.Range("FlagZ").Value = "0"
    wsCPU.Range("FlagC").Value = "0"
    wsCPU.Range("FlagS").Value = "0"
    
    ' 3. Restaurar Interfaz de Memoria
    wsCPU.Range("RDM").Value = "000"
    wsCPU.Range("RIM").Value = "0"
    wsCPU.Range("SelectorMem").Value = "INACTIVO"
    wsCPU.Range("BusDirecciones").Value = "C.P. / R.I."
    wsCPU.Range("BusDatos").Value = "RIM <-> RAM"
    wsCPU.Range("BusControl").Value = "LECTURA / ESCRITURA"
    wsCPU.Range("Reloj").Value = "T0"
    
    ' 4. Restaurar Memoria Central original
    Dim rngMem As Range
    Set rngMem = wsCPU.Range("TablaMemoria")
    rngMem.Cells(1, 1).Value = "001": rngMem.Cells(1, 2).Value = "0"
    rngMem.Cells(2, 1).Value = "006": rngMem.Cells(2, 2).Value = "SUMAR 033 992 993"
    rngMem.Cells(3, 1).Value = "007": rngMem.Cells(3, 2).Value = "HLT"
    rngMem.Cells(4, 1).Value = "033": rngMem.Cells(4, 2).Value = "56789"
    rngMem.Cells(5, 1).Value = "992": rngMem.Cells(5, 2).Value = "66666"
    rngMem.Cells(6, 1).Value = "993": rngMem.Cells(6, 2).Value = "0"
    
    ' Limpiar punteros en columna F
    wsCPU.Range("F15:F25").ClearContents
    wsCPU.Range("F16").Value = "◄ C.P. Inicial"
    wsCPU.Range("F18").Value = "◄ &OP1"
    wsCPU.Range("F19").Value = "◄ &OP2"
    wsCPU.Range("F20").Value = "◄ &OP3 Destino"
    
    ' 5. Limpiar Consola
    wsCPU.Range("Consola").Value = "Simulador reiniciado a condiciones iniciales." & vbCrLf & _
                                   "C.P. = 006 | Presione 'Paso a Paso' o 'Ejecutar Instrucción'."
                                   
    ' 6. Limpiar Tabla de Traza
    Dim ultFilaTraza As Long
    ultFilaTraza = wsTraza.Cells(wsTraza.Rows.Count, 1).End(xlUp).Row
    If ultFilaTraza > 4 Then
        wsTraza.Range("A5:K" & ultFilaTraza).Clear
    End If
    wsTraza.Range("A4:K4").Value = Array("T0", "INICIAL", "Estado previo al inicio", "006", "000", "0", "0", "0", "0", "0", "0")
    
    ' 7. Restaurar colores neutros de flechas
    Dim shp As Shape
    For Each shp In wsCPU.Shapes
        If Left(shp.Name, 6) = "Flecha" Then
            shp.Line.ForeColor.RGB = COLOR_FLECHA_NEUTRA
            shp.Line.Weight = 1.5
        End If
    Next shp
    
    wsCPU.Range("Consola").Value = "Simulador reiniciado exitosamente a condiciones iniciales (CP=006). Presione Paso a Paso o Ejecutar."
    Reproteger wsCPU
End Sub

' ==============================================================================
' 5. FUNCIONES DE MEMORIA Y DECODIFICACIÓN
' ==============================================================================
Public Function BuscarEnMemoria(ByVal direccion As String) As String
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    
    Dim rngMem As Range
    Set rngMem = ws.Range("TablaMemoria")
    
    Dim sDirBuscada As String
    sDirBuscada = Format(Val(Trim(direccion)), "000")
    
    Dim i As Long
    For i = 1 To rngMem.Rows.Count
        Dim sDirFila As String
        sDirFila = Format(Val(Trim(rngMem.Cells(i, 1).Value)), "000")
        If sDirFila = sDirBuscada Then
            BuscarEnMemoria = Trim(CStr(rngMem.Cells(i, 2).Value))
            Dim rActual As Long
            rActual = rngMem.Cells(i, 1).Row
            ws.Range("F15:F25").ClearContents
            ws.Cells(rActual, 6).Value = "◄ Activo"
            Exit Function
        End If
    Next i
    
    BuscarEnMemoria = "ERROR: Dirección no encontrada"
    ws.Range("Consola").Value = "ERROR: Dirección de memoria " & direccion & " no encontrada en RAM."
End Function

Public Sub EscribirEnMemoria(ByVal direccion As String, ByVal valor As String)
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    Desproteger ws
    
    Dim rngMem As Range
    Set rngMem = ws.Range("TablaMemoria")
    
    Dim sDirBuscada As String
    sDirBuscada = Format(Val(Trim(direccion)), "000")
    
    Dim i As Long
    For i = 1 To rngMem.Rows.Count
        Dim sDirFila As String
        sDirFila = Format(Val(Trim(rngMem.Cells(i, 1).Value)), "000")
        If sDirFila = sDirBuscada Then
            rngMem.Cells(i, 2).Value = valor
            ResaltarCambio rngMem.Cells(i, 2)
            
            Dim rActual As Long
            rActual = rngMem.Cells(i, 1).Row
            ws.Range("F15:F25").ClearContents
            ws.Cells(rActual, 6).Value = "◄ Modificado"
            Exit Sub
        End If
    Next i
    
    ws.Range("Consola").Value = "ERROR: No se pudo escribir en Mem[" & direccion & "]; dirección no encontrada."
End Sub

Public Sub DecodificarInstruccion()
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    
    Dim sRI As String
    sRI = Trim(ws.Range("RI").Value)
    
    ws.Range("CO").Value = ""
    ws.Range("_OP1").Value = ""
    ws.Range("_OP2").Value = ""
    ws.Range("_OP3").Value = ""
    
    If sRI = "" Then Exit Sub
    
    Dim partes() As String
    partes = Split(sRI, " ")
    
    If UBound(partes) >= 0 Then ws.Range("CO").Value = UCase(partes(0))
    If UBound(partes) >= 1 Then ws.Range("_OP1").Value = partes(1)
    If UBound(partes) >= 2 Then ws.Range("_OP2").Value = partes(2)
    If UBound(partes) >= 3 Then ws.Range("_OP3").Value = partes(3)
End Sub

' ==============================================================================
' 6. REGISTRO EN TABLA DE TRAZA
' ==============================================================================
Public Sub RegistrarPasoEnTraza(ByVal notacionRTL As String)
    Dim wsCPU As Worksheet, wsTraza As Worksheet
    Set wsCPU = ThisWorkbook.Sheets("Simulador_CPU")
    Set wsTraza = ThisWorkbook.Sheets("Tabla_de_Traza")
    
    Dim sigFila As Long
    sigFila = wsTraza.Cells(wsTraza.Rows.Count, 1).End(xlUp).Row + 1
    If sigFila < 4 Then sigFila = 4
    
    Dim sOP3 As String, valMemOP3 As String
    sOP3 = Trim(wsCPU.Range("_OP3").Value)
    If sOP3 <> "" Then
        valMemOP3 = BuscarEnMemoria(sOP3)
    Else
        valMemOP3 = "-"
    End If
    
    wsTraza.Cells(sigFila, 1).Value = wsCPU.Range("Reloj").Value
    wsTraza.Cells(sigFila, 2).Value = wsCPU.Range("Fase").Value
    wsTraza.Cells(sigFila, 3).Value = notacionRTL
    wsTraza.Cells(sigFila, 4).Value = wsCPU.Range("CP").Value
    wsTraza.Cells(sigFila, 5).Value = wsCPU.Range("RDM").Value
    wsTraza.Cells(sigFila, 6).Value = wsCPU.Range("RIM").Value
    wsTraza.Cells(sigFila, 7).Value = wsCPU.Range("RI").Value
    wsTraza.Cells(sigFila, 8).Value = wsCPU.Range("_REN1").Value
    wsTraza.Cells(sigFila, 9).Value = wsCPU.Range("_REN2").Value
    wsTraza.Cells(sigFila, 10).Value = wsCPU.Range("Ac").Value
    wsTraza.Cells(sigFila, 11).Value = valMemOP3
    
    With wsTraza.Range(wsTraza.Cells(sigFila, 1), wsTraza.Cells(sigFila, 11))
        .HorizontalAlignment = xlCenter
        .VerticalAlignment = xlCenter
        .Borders.LineStyle = xlContinuous
        .Borders.Color = RGB(210, 210, 210)
        .Font.Name = "Segoe UI"
        .Font.Size = 9
    End With
    wsTraza.Cells(sigFila, 3).HorizontalAlignment = xlLeft
End Sub

' ==============================================================================
' 7. ANIMACIÓN DE FLECHAS Y RESALTADO DE CELDAS
' ==============================================================================
Public Sub ResaltarCambio(ByVal rng As Range)
    On Error Resume Next
    Dim colorOriginal As Long
    colorOriginal = rng.Interior.Color
    
    rng.Interior.Color = COLOR_RESALTADO
    DoEvents
    
    PausaSegundos RETARDO_MICROORDEN
    
    rng.Interior.Color = colorOriginal
    DoEvents
End Sub

Public Sub AnimarFlecha(ByVal nombreFlecha As String)
    On Error Resume Next
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    
    Dim shp As Shape
    Set shp = ws.Shapes(nombreFlecha)
    If shp Is Nothing Then Exit Sub
    
    Dim pesoOriginal As Single
    Dim colorOriginal As Long
    pesoOriginal = shp.Line.Weight
    colorOriginal = shp.Line.ForeColor.RGB
    
    shp.Line.ForeColor.RGB = COLOR_FLECHA_ACTIVA
    shp.Line.Weight = 3.5
    DoEvents
    
    PausaSegundos RETARDO_MICROORDEN
    
    shp.Line.ForeColor.RGB = colorOriginal
    shp.Line.Weight = pesoOriginal
    DoEvents
End Sub

Private Sub ActualizarBanderas(ByVal resultado As Double)
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")
    
    If resultado = 0 Then
        ws.Range("FlagZ").Value = 1
    Else
        ws.Range("FlagZ").Value = 0
    End If
    
    If resultado < 0 Then
        ws.Range("FlagS").Value = 1
    Else
        ws.Range("FlagS").Value = 0
    End If
    
    If Abs(resultado) > 65535 Then
        ws.Range("FlagC").Value = 1
    Else
        ws.Range("FlagC").Value = 0
    End If
End Sub

Public Sub PausaSegundos(ByVal seg As Double)
    Dim tInicio As Double
    tInicio = Timer
    Do While Timer < tInicio + seg
        DoEvents
    Loop
End Sub

' ==============================================================================
' GESTIÓN DE PROTECCIÓN DE HOJA (Permite edición de usuario solo en B15:E20)
' ==============================================================================
Private Sub Desproteger(ws As Worksheet)
    On Error Resume Next
    ws.Unprotect ""
End Sub

Private Sub Reproteger(ws As Worksheet)
    On Error Resume Next
    ws.Range("B15:E20").Locked = False
    ws.Protect Password:="", DrawingObjects:=False, Contents:=True, Scenarios:=True, UserInterfaceOnly:=True
End Sub
