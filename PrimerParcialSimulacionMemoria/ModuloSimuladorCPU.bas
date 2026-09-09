Attribute VB_Name = "ModuloSimuladorCPU"
' ==============================================================================
' SIMULADOR DE ARQUITECTURA DE COMPUTADORAS EN EXCEL VBA
' Modelo: Eduardo Alcalde Lancharro & Miguel García López (McGraw-Hill)
' Instrucción con 3 Direcciones: SUMAR 033 992 993
' ==============================================================================
Option Explicit

' Constantes de celdas en la hoja 'Simulador_CPU'
Public Const CELDA_CP       As String = "C5"
Public Const CELDA_RI       As String = "C6"
Public Const CELDA_CO       As String = "C7"
Public Const CELDA_OP1_ADDR As String = "C8"
Public Const CELDA_OP2_ADDR As String = "C9"
Public Const CELDA_OP3_ADDR As String = "C10"
Public Const CELDA_ESTADO   As String = "C11"

Public Const CELDA_REN1     As String = "G5"
Public Const CELDA_REN2     As String = "G6"
Public Const CELDA_COP      As String = "G7"
Public Const CELDA_AC       As String = "G8"
Public Const CELDA_ZF       As String = "G9"
Public Const CELDA_CF       As String = "G10"
Public Const CELDA_SF       As String = "G11"

Public Const CELDA_RDM      As String = "K5"
Public Const CELDA_RIM      As String = "K6"
Public Const CELDA_SELECTOR As String = "K7"
Public Const CELDA_PASO_TN  As String = "K11"

Public Const CELDA_CONSOLA  As String = "H18"

' ------------------------------------------------------------------------------
' 1. EJECUCIÓN PASO A PASO (UN PULSO DE RELOJ = UNA MICROORDEN)
' ------------------------------------------------------------------------------
Public Sub PasoCicloReloj()
    Dim wsSim As Worksheet, wsTraza As Worksheet
    Set wsSim = ThisWorkbook.Sheets("Simulador_CPU")
    On Error Resume Next
    Set wsTraza = ThisWorkbook.Sheets("Tabla_de_Traza")
    On Error GoTo 0

    Dim paso As String
    paso = UCase(Trim(CStr(wsSim.Range(CELDA_PASO_TN).Value)))
    If paso = "" Then paso = "T0"

    Dim cp As String, rdm As String, rim As String, ri As String
    Dim ren1 As String, ren2 As String, ac As String
    cp = CStr(wsSim.Range(CELDA_CP).Value)
    rdm = CStr(wsSim.Range(CELDA_RDM).Value)
    rim = CStr(wsSim.Range(CELDA_RIM).Value)
    ri = CStr(wsSim.Range(CELDA_RI).Value)
    ren1 = CStr(wsSim.Range(CELDA_REN1).Value)
    ren2 = CStr(wsSim.Range(CELDA_REN2).Value)
    ac = CStr(wsSim.Range(CELDA_AC).Value)

    Dim nuevoPaso As String, fase As String, microorden As String, descripcion As String

    Select Case paso
        Case "T0"
            nuevoPaso = "T1"
            fase = "BÚSQUEDA"
            microorden = "RDM ← C.P."
            rdm = cp
            descripcion = "Paso T1: La Unidad de Control transfiere el contenido del C.P. (" & cp & ") al RDM a través del bus de direcciones."
            wsSim.Range(CELDA_ESTADO).Value = "BÚSQUEDA (Fetch)"
            wsSim.Range(CELDA_RDM).Value = rdm
            wsSim.Range(CELDA_SELECTOR).Value = "APUNTANDO A " & rdm
            ResaltarFilaMemoria wsSim, "006", RGB(254, 240, 138) ' Amarillo

        Case "T1"
            nuevoPaso = "T2"
            fase = "BÚSQUEDA"
            microorden = "RIM ← M(RDM)"
            rim = ObtenerDatoMemoria(wsSim, rdm)
            descripcion = "Paso T2: El selector accede a la dirección " & rdm & " de la Memoria Central y transfiere la instrucción al RIM."
            wsSim.Range(CELDA_RIM).Value = rim

        Case "T2"
            nuevoPaso = "T3"
            fase = "BÚSQUEDA"
            microorden = "R.I. ← RIM"
            ri = rim
            descripcion = "Paso T3: La instrucción se transfiere del RIM al Registro de Instrucción (R.I.) en la Unidad de Control."
            wsSim.Range(CELDA_RI).Value = ri

        Case "T3"
            nuevoPaso = "T4"
            fase = "BÚSQUEDA"
            microorden = "Sec ← Decod(RI)"
            descripcion = "Paso T4: El Decodificador interpreta la instrucción '" & ri & "' y notifica al Secuenciador."
            wsSim.Range(CELDA_CO).Value = "SUMAR"
            wsSim.Range(CELDA_OP1_ADDR).Value = "033"
            wsSim.Range(CELDA_OP2_ADDR).Value = "992"
            wsSim.Range(CELDA_OP3_ADDR).Value = "993"

        Case "T4"
            nuevoPaso = "T5"
            fase = "BÚSQUEDA"
            microorden = "C.P. ← C.P. + 1"
            cp = "007"
            descripcion = "Paso T5: El Contador de Programa (C.P.) se autoincrementa en 1, apuntando a la siguiente posición consecutiva (007)."
            wsSim.Range(CELDA_CP).Value = cp

        Case "T5"
            nuevoPaso = "T6"
            fase = "EJECUCIÓN"
            microorden = "RDM ← RI(&OP1)"
            rdm = "033"
            descripcion = "Paso T6: Se transfiere la dirección del 1er Operando (033) desde el R.I. al RDM."
            wsSim.Range(CELDA_ESTADO).Value = "EJECUCIÓN (Execute)"
            wsSim.Range(CELDA_RDM).Value = rdm
            wsSim.Range(CELDA_SELECTOR).Value = "APUNTANDO A 033"
            ResaltarFilaMemoria wsSim, "033", RGB(191, 219, 254) ' Azul

        Case "T6"
            nuevoPaso = "T7"
            fase = "EJECUCIÓN"
            microorden = "RIM ← S(RDM)"
            rim = ObtenerDatoMemoria(wsSim, rdm)
            descripcion = "Paso T7: El selector extrae el dato del 1er operando (" & rim & ") de la memoria y lo carga en el RIM."
            wsSim.Range(CELDA_RIM).Value = rim

        Case "T7"
            nuevoPaso = "T8"
            fase = "EJECUCIÓN"
            microorden = "REN 1 ← RIM"
            ren1 = rim
            descripcion = "Paso T8: El dato del 1er operando (" & ren1 & ") viaja desde el RIM al Registro de Entrada 1 (REN 1) de la UAL."
            wsSim.Range(CELDA_REN1).Value = ren1

        Case "T8"
            nuevoPaso = "T9"
            fase = "EJECUCIÓN"
            microorden = "RDM ← RI(&OP2)"
            rdm = "992"
            descripcion = "Paso T9: Se transfiere la dirección del 2do Operando (992) desde el R.I. al RDM."
            wsSim.Range(CELDA_RDM).Value = rdm
            wsSim.Range(CELDA_SELECTOR).Value = "APUNTANDO A 992"
            ResaltarFilaMemoria wsSim, "992", RGB(191, 219, 254)

        Case "T9"
            nuevoPaso = "T10"
            fase = "EJECUCIÓN"
            microorden = "RIM ← S(RDM)"
            rim = ObtenerDatoMemoria(wsSim, rdm)
            descripcion = "Paso T10: El selector extrae el dato del 2do operando (" & rim & ") de la memoria y lo carga en el RIM."
            wsSim.Range(CELDA_RIM).Value = rim

        Case "T10"
            nuevoPaso = "T11"
            fase = "EJECUCIÓN"
            microorden = "REN 2 ← RIM"
            ren2 = rim
            descripcion = "Paso T11: El dato del 2do operando (" & ren2 & ") viaja desde el RIM al Registro de Entrada 2 (REN 2) de la UAL."
            wsSim.Range(CELDA_REN2).Value = ren2

        Case "T11"
            nuevoPaso = "T12"
            fase = "EJECUCIÓN"
            microorden = "Ac ← C.OP(REN1, REN2)"
            Dim valSuma As Double
            valSuma = CDbl(ren1) + CDbl(ren2)
            ac = CStr(valSuma)
            descripcion = "Paso T12: La UAL suma REN 1 + REN 2 (" & ren1 & " + " & ren2 & " = " & ac & "). El resultado se guarda en el Acumulador."
            wsSim.Range(CELDA_AC).Value = ac
            wsSim.Range(CELDA_ZF).Value = IIf(valSuma = 0, 1, 0)

        Case "T12"
            nuevoPaso = "T13"
            fase = "EJECUCIÓN"
            microorden = "RIM ← Ac"
            rim = ac
            descripcion = "Paso T13: El resultado de la suma (" & ac & ") viaja desde el Acumulador al RIM por el bus de datos."
            wsSim.Range(CELDA_RIM).Value = rim

        Case "T13"
            nuevoPaso = "T14"
            fase = "EJECUCIÓN"
            microorden = "RDM ← RI(&OP3)"
            rdm = "993"
            descripcion = "Paso T14: Se transfiere la dirección de destino (993) desde el R.I. al RDM."
            wsSim.Range(CELDA_RDM).Value = rdm
            wsSim.Range(CELDA_SELECTOR).Value = "APUNTANDO A 993"
            ResaltarFilaMemoria wsSim, "993", RGB(187, 247, 208) ' Verde

        Case "T14"
            nuevoPaso = "T15"
            fase = "EJECUCIÓN"
            microorden = "M(RDM) ← RIM"
            EscribirDatoMemoria wsSim, rdm, rim
            descripcion = "Paso T15: El selector graba el dato del RIM (" & rim & ") en la celda 993 de la Memoria Central. ¡Instrucción completada con éxito!"
            wsSim.Range(CELDA_ESTADO).Value = "COMPLETADO (Fin de Ciclo)"
            wsSim.Range(CELDA_SELECTOR).Value = "ESCRITURA COMPLETADA"

        Case "T15"
            MsgBox "Instrucción finalizada. Presione 'Reset' para reiniciar.", vbInformation, "Simulador CPU"
            Exit Sub
    End Select

    wsSim.Range(CELDA_PASO_TN).Value = nuevoPaso
    wsSim.Range(CELDA_CONSOLA).Value = descripcion

    If Not wsTraza Is Nothing Then
        Dim nextRow As Long
        nextRow = wsTraza.Cells(wsTraza.Rows.Count, "A").End(xlUp).Row + 1
        Dim mem993 As String
        mem993 = ObtenerDatoMemoria(wsSim, "993")
        wsTraza.Cells(nextRow, 1).Value = nuevoPaso
        wsTraza.Cells(nextRow, 2).Value = fase
        wsTraza.Cells(nextRow, 3).Value = microorden
        wsTraza.Cells(nextRow, 4).Value = cp
        wsTraza.Cells(nextRow, 5).Value = rdm
        wsTraza.Cells(nextRow, 6).Value = rim
        wsTraza.Cells(nextRow, 7).Value = ri
        wsTraza.Cells(nextRow, 8).Value = ren1
        wsTraza.Cells(nextRow, 9).Value = ren2
        wsTraza.Cells(nextRow, 10).Value = ac
        wsTraza.Cells(nextRow, 11).Value = mem993
        wsTraza.Cells(nextRow, 12).Value = descripcion
    End If
End Sub

' ------------------------------------------------------------------------------
' 2. EJECUCIÓN COMPLETA AUTOMATIZADA CON EFECTO DE ANIMACIÓN
' ------------------------------------------------------------------------------
Public Sub EjecutarInstruccionCompleta()
    ResetCPU
    Dim i As Integer
    For i = 1 To 15
        PasoCicloReloj
        DoEvents
        Application.Wait (Now + TimeValue("0:00:01")) ' Pausa de 1 segundo para visualización
    Next i
End Sub

' ------------------------------------------------------------------------------
' 3. REINICIO DE REGISTROS Y MEMORIA
' ------------------------------------------------------------------------------
Public Sub ResetCPU()
    Dim wsSim As Worksheet, wsTraza As Worksheet
    Set wsSim = ThisWorkbook.Sheets("Simulador_CPU")
    On Error Resume Next
    Set wsTraza = ThisWorkbook.Sheets("Tabla_de_Traza")
    On Error GoTo 0

    wsSim.Range(CELDA_CP).Value = "006"
    wsSim.Range(CELDA_RI).Value = "SUMAR 033 992 993"
    wsSim.Range(CELDA_CO).Value = "SUMAR"
    wsSim.Range(CELDA_OP1_ADDR).Value = "033"
    wsSim.Range(CELDA_OP2_ADDR).Value = "992"
    wsSim.Range(CELDA_OP3_ADDR).Value = "993"
    wsSim.Range(CELDA_ESTADO).Value = "INICIO"

    wsSim.Range(CELDA_REN1).Value = "0"
    wsSim.Range(CELDA_REN2).Value = "0"
    wsSim.Range(CELDA_COP).Value = "SUMA (+)"
    wsSim.Range(CELDA_AC).Value = "0"
    wsSim.Range(CELDA_ZF).Value = "0"
    wsSim.Range(CELDA_CF).Value = "0"
    wsSim.Range(CELDA_SF).Value = "0"

    wsSim.Range(CELDA_RDM).Value = "000"
    wsSim.Range(CELDA_RIM).Value = "0"
    wsSim.Range(CELDA_SELECTOR).Value = "INACTIVO"
    wsSim.Range(CELDA_PASO_TN).Value = "T0"

    EscribirDatoMemoria wsSim, "993", "0"

    wsSim.Range("B15:F20").Interior.Color = RGB(255, 255, 255)
    wsSim.Range("B16:F16").Interior.Color = RGB(254, 240, 138) ' Resaltar 006

    wsSim.Range(CELDA_CONSOLA).Value = "CPU Reiniciada." & vbCrLf & _
        "Presione 'Paso a Paso' para ejecutar la Microorden 1 (RDM <- C.P.)."

    If Not wsTraza Is Nothing Then
        Dim lastRow As Long
        lastRow = wsTraza.Cells(wsTraza.Rows.Count, "A").End(xlUp).Row
        If lastRow > 4 Then
            wsTraza.Range("A5:L" & lastRow).ClearContents
        End If
    End If
End Sub

' ------------------------------------------------------------------------------
' 4. FUNCIONES AUXILIARES DE ACCESO A MEMORIA
' ------------------------------------------------------------------------------
Private Function ObtenerDatoMemoria(ws As Worksheet, ByVal direccion As String) As String
    Dim i As Long
    For i = 15 To 20
        If Trim(CStr(ws.Cells(i, 2).Value)) = Trim(direccion) Then
            ObtenerDatoMemoria = CStr(ws.Cells(i, 3).Value)
            Exit Function
        End If
    Next i
    ObtenerDatoMemoria = "0"
End Function

Private Sub EscribirDatoMemoria(ws As Worksheet, ByVal direccion As String, ByVal valor As String)
    Dim i As Long
    For i = 15 To 20
        If Trim(CStr(ws.Cells(i, 2).Value)) = Trim(direccion) Then
            ws.Cells(i, 3).Value = valor
            Exit Sub
        End If
    Next i
End Sub

Private Sub ResaltarFilaMemoria(ws As Worksheet, ByVal direccion As String, ByVal colorRGB As Long)
    ws.Range("B15:F20").Interior.Color = RGB(255, 255, 255)
    Dim i As Long
    For i = 15 To 20
        If Trim(CStr(ws.Cells(i, 2).Value)) = Trim(direccion) Then
            ws.Range(ws.Cells(i, 2), ws.Cells(i, 6)).Interior.Color = colorRGB
            Exit Sub
        End If
    Next i
End Sub

' ------------------------------------------------------------------------------
' 5. CREACIÓN AUTOMÁTICA DE BOTONES GRÁFICOS FLOTANTES CON FORMAS (SHAPES)
' ------------------------------------------------------------------------------
Public Sub CrearBotonesInteractivos()
    Dim ws As Worksheet
    Set ws = ThisWorkbook.Sheets("Simulador_CPU")

    ' Eliminar botones previos si existen
    Dim shp As Shape
    For Each shp In ws.Shapes
        If shp.Name Like "BtnSim_*" Then shp.Delete
    Next shp

    ' Botón 1: Paso a Paso
    Dim b1 As Shape
    Set b1 = ws.Shapes.AddShape(msoShapeRoundedRectangle, ws.Range("H14").Left, ws.Range("H14").Top, 110, 26)
    b1.Name = "BtnSim_Paso"
    b1.TextFrame.Characters.Text = "▶ Paso a Paso"
    b1.TextFrame.Characters.Font.Bold = True
    b1.TextFrame.Characters.Font.Color = RGB(255, 255, 255)
    b1.Fill.Solid
    b1.Fill.ForeColor.RGB = RGB(37, 99, 235) ' Azul
    b1.OnAction = "PasoCicloReloj"

    ' Botón 2: Reset
    Dim b2 As Shape
    Set b2 = ws.Shapes.AddShape(msoShapeRoundedRectangle, ws.Range("J14").Left, ws.Range("J14").Top, 110, 26)
    b2.Name = "BtnSim_Reset"
    b2.TextFrame.Characters.Text = "🔄 Reset CPU"
    b2.TextFrame.Characters.Font.Bold = True
    b2.TextFrame.Characters.Font.Color = RGB(255, 255, 255)
    b2.Fill.Solid
    b2.Fill.ForeColor.RGB = RGB(239, 68, 68) ' Rojo
    b2.OnAction = "ResetCPU"

    ' Botón 3: Auto-Play
    Dim b3 As Shape
    Set b3 = ws.Shapes.AddShape(msoShapeRoundedRectangle, ws.Range("H15").Left, ws.Range("H15").Top, 230, 26)
    b3.Name = "BtnSim_Auto"
    b3.TextFrame.Characters.Text = "⏩ Ejecutar Instrucción Completa"
    b3.TextFrame.Characters.Font.Bold = True
    b3.TextFrame.Characters.Font.Color = RGB(255, 255, 255)
    b3.Fill.Solid
    b3.Fill.ForeColor.RGB = RGB(16, 185, 129) ' Verde
    b3.OnAction = "EjecutarInstruccionCompleta"

    MsgBox "¡Botones interactivos creados con éxito en la hoja!", vbInformation, "Simulador CPU"
End Sub
