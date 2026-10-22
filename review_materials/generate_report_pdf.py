import os
from reportlab.lib.pagesizes import A4
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak
)
from reportlab.pdfgen import canvas

class NumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        super(NumberedCanvas, self).__init__(*args, **kwargs)
        self._saved_page_states = []

    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()

    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.draw_page_number(num_pages)
            super(NumberedCanvas, self).showPage()
        super(NumberedCanvas, self).save()

    def draw_page_number(self, page_count):
        if self._pageNumber > 1:
            self.saveState()
            self.setFont("Helvetica", 9)
            self.setFillColor(colors.HexColor("#64748b"))
            self.setStrokeColor(colors.HexColor("#cbd5e1"))
            self.setLineWidth(0.5)
            self.line(45, 40, 550, 40)
            self.drawString(45, 28, "Compiler Design Lab | Project Review 2 Report (75% Milestone)")
            page_text = f"Page {self._pageNumber} of {page_count}"
            self.drawRightString(550, 28, page_text)
            self.restoreState()

def create_report():
    pdf_filename = "CD_LAB_ProjRev2.pdf"
    doc = SimpleDocTemplate(
        pdf_filename,
        pagesize=A4,
        leftMargin=45,
        rightMargin=45,
        topMargin=45,
        bottomMargin=55
    )

    styles = getSampleStyleSheet()

    # Custom typography matching Review 1
    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=20,
        leading=26,
        alignment=1, # Center
        textColor=colors.HexColor("#0f172a")
    )
    
    subtitle_style = ParagraphStyle(
        'DocSubTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=15,
        leading=22,
        alignment=1,
        textColor=colors.HexColor("#1e293b")
    )

    h1_style = ParagraphStyle(
        'H1',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=12,
        leading=16,
        textColor=colors.HexColor("#0f172a"),
        spaceAfter=5,
        spaceBefore=7
    )

    h2_style = ParagraphStyle(
        'H2',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=9.5,
        leading=13,
        textColor=colors.HexColor("#1e293b"),
        spaceAfter=3,
        spaceBefore=5
    )

    body_style = ParagraphStyle(
        'Body',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=12,
        textColor=colors.HexColor("#334155"),
        spaceAfter=3
    )

    bullet_style = ParagraphStyle(
        'Bullet',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=11.5,
        textColor=colors.HexColor("#334155"),
        leftIndent=14,
        spaceAfter=2
    )

    table_header_style = ParagraphStyle(
        'TableHeader',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=8,
        leading=10,
        textColor=colors.HexColor("#0f172a")
    )

    table_body_style = ParagraphStyle(
        'TableBody',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=7.5,
        leading=9.5,
        textColor=colors.HexColor("#1e293b")
    )

    code_style = ParagraphStyle(
        'CodeStyle',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=7,
        leading=9,
        textColor=colors.HexColor("#0f172a")
    )

    terminal_code_style = ParagraphStyle(
        'TerminalCode',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=6.5,
        leading=8.2,
        textColor=colors.HexColor("#f8fafc")
    )

    story = []

    # =========================================================================
    # PAGE 1: TITLE PAGE
    # =========================================================================
    story.append(Spacer(1, 140))
    story.append(Paragraph("Compiler Design Lab – Project Review 2<br/>Report", title_style))
    story.append(Spacer(1, 60))
    story.append(Paragraph("Project Title:", subtitle_style))
    story.append(Spacer(1, 12))
    story.append(Paragraph("DAG-Based Optimization for Basic Blocks", title_style))
    story.append(Spacer(1, 24))
    
    badge_data = [[
        Paragraph("<font color='#065f46'><b>Implementation Progress: 75% Complete (Review 2)</b></font>", 
                  ParagraphStyle('Bdg', fontName='Helvetica-Bold', fontSize=10, alignment=1))
    ]]
    badge_table = Table(badge_data, colWidths=[360])
    badge_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#dcfce7")),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor("#10b981")),
        ('TOPPADDING', (0,0), (-1,-1), 6),
        ('BOTTOMPADDING', (0,0), (-1,-1), 6),
        ('ALIGN', (0,0), (-1,-1), 'CENTER'),
    ]))
    story.append(badge_table)
    story.append(Spacer(1, 80))

    story.append(Paragraph("<b>Team Members:</b>", ParagraphStyle('TM', fontName='Helvetica-Bold', fontSize=12, alignment=1, textColor=colors.HexColor("#0f172a"))))
    story.append(Spacer(1, 12))
    story.append(Paragraph("• Abhinav Gupta (24BCT0185)", ParagraphStyle('TMM', fontName='Helvetica', fontSize=11, alignment=1, leading=18)))
    story.append(Paragraph("• Shobit Khanna (24BCT0187)", ParagraphStyle('TMM', fontName='Helvetica', fontSize=11, alignment=1, leading=18)))
    story.append(Paragraph("• Arnav Goel (24BCT0189)", ParagraphStyle('TMM', fontName='Helvetica', fontSize=11, alignment=1, leading=18)))
    story.append(PageBreak())

    # =========================================================================
    # PAGE 2: PROBLEM STATEMENT & OBJECTIVES
    # =========================================================================
    story.append(Paragraph("1. Problem Statement", h1_style))
    story.append(Paragraph(
        "Unoptimized intermediate code, such as standard Three-Address Code (TAC) generated by compiler frontends, "
        "frequently contains redundant computations, duplicate subexpressions, and dead operations. Values are repeatedly "
        "recomputed across a basic block, unnecessarily wasting CPU cycles, memory bandwidth, and execution time. "
        "Our goal is to build an optimizing compiler engine that constructs a Directed Acyclic Graph (DAG) for basic blocks "
        "to automatically detect, fold, simplify, and eliminate these redundancies while synthesizing valid, optimized TAC.",
        body_style
    ))
    story.append(Spacer(1, 2))

    story.append(Paragraph("<b>Key Challenges Addressed:</b>", h2_style))
    story.append(Paragraph("• <b>Constant-Time Redundancy Detection:</b> Eliminating duplicate calculations in O(1) time without costly full-graph traversals.", bullet_style))
    story.append(Paragraph("• <b>Commutative Operations:</b> Recognizing semantic equivalence for commutative operations (e.g., a + b == b + a).", bullet_style))
    story.append(Paragraph("• <b>Variable Reassignment & Aliasing:</b> Handling Dragon Book node kill and label detachment when variables are overwritten.", bullet_style))
    story.append(Paragraph("• <b>Compile-Time Constant Evaluation:</b> Folding static arithmetic and propagating constant values through the graph.", bullet_style))
    story.append(Paragraph("• <b>Algebraic Reductions:</b> Identifying mathematical identities (x + 0, x * 1, x * 0, x - x) for strength reduction.", bullet_style))
    story.append(Paragraph("• <b>Code Generation:</b> Reconstructing shortened, valid Three-Address Code and Graphviz visualizations from the DAG.", bullet_style))
    story.append(Spacer(1, 4))

    story.append(Paragraph("<b>Naive Input vs Optimized Output:</b>", h2_style))
    io_data = [
        [Paragraph("<b>Unoptimized TAC (Before)</b>", table_header_style), Paragraph("<b>Optimized TAC (After)</b>", table_header_style), Paragraph("<b>Optimization Applied</b>", table_header_style)],
        [Paragraph("t1 = a * b<br/>t2 = a * b<br/>t3 = c + d<br/>t4 = t3 - t1<br/>t5 = c + d<br/>t6 = t5 - t2", code_style),
         Paragraph("t1 = a * b<br/>t2 = t1<br/>t3 = c + d<br/>t4 = t3 - t1<br/>t5 = t3<br/>t6 = t4", code_style),
         Paragraph("Initial Computation<br/><b>CSE:</b> Reuses Node t1<br/>Initial Computation<br/>Computation<br/><b>CSE:</b> Reuses Node t3<br/><b>CSE:</b> Transitive Subexpression", table_body_style)]
    ]
    io_table = Table(io_data, colWidths=[150, 150, 205])
    io_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#f1f5f9")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#94a3b8")),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(io_table)
    story.append(Spacer(1, 6))

    story.append(Paragraph("2. Objectives", h1_style))
    story.append(Paragraph("<b>Review 1 Objectives (25% - Completed Recap):</b>", h2_style))
    story.append(Paragraph("• Establish core data structures for DAG nodes and leaf representations.", bullet_style))
    story.append(Paragraph("• Implement Value Numbering using hash maps for constant-time O(1) duplicate detection.", bullet_style))
    story.append(Paragraph("• Demonstrate proof-of-concept graph construction on a toy hardcoded 4-instruction basic block.", bullet_style))
    story.append(Spacer(1, 2))

    story.append(Paragraph("<b>Review 2 Objectives (75% - Implemented & Delivered):</b>", h2_style))
    story.append(Paragraph("• <b>Dynamic TAC File Parser:</b> Replace hardcoded strings with a robust file parser handling binary, unary, copy, and constants.", bullet_style))
    story.append(Paragraph("• <b>Modular Architectural Refactoring:</b> Restructure into production-grade C++ headers (include/) and implementations (src/).", bullet_style))
    story.append(Paragraph("• <b>Commutative CSE:</b> Canonicalize commutative operators (+, *, &, |, ^, ==, !=) using sorted operand IDs.", bullet_style))
    story.append(Paragraph("• <b>Node Kill & Reassignment:</b> Implement strict Dragon Book label detachment when variables are overwritten.", bullet_style))
    story.append(Paragraph("• <b>Constant Folding & Algebraic Identities:</b> Static compile-time evaluation and neutral/annihilator reduction.", bullet_style))
    story.append(Paragraph("• <b>Optimized TAC Code Generator:</b> Traverse and replay DAG to emit clean, shortened TAC files.", bullet_style))
    story.append(Paragraph("• <b>Visual Graphviz Exporter:</b> Generate .dot graph files for graphical DAG rendering.", bullet_style))
    story.append(Paragraph("• <b>Empirical Benchmarks:</b> Validate against 5 comprehensive benchmark test suites with detailed metrics.", bullet_style))
    story.append(PageBreak())

    # =========================================================================
    # PAGE 3: LITERATURE SURVEY & SYSTEM DESIGN
    # =========================================================================
    story.append(Paragraph("3. Literature Survey", h1_style))
    lit_data = [
        [Paragraph("<b>Theme</b>", table_header_style), Paragraph("<b>Books / Papers</b>", table_header_style), Paragraph("<b>Key Idea</b>", table_header_style), Paragraph("<b>Relevance to this Project</b>", table_header_style)],
        [Paragraph("Foundations", table_body_style),
         Paragraph("A. V. Aho, M. S. Lam, R. Sethi, J. D. Ullman (2007). <i>Compilers: Principles, Techniques, & Tools</i>.", table_body_style),
         Paragraph("Constructing DAGs for basic blocks; variable reassignment and label kill rules.", table_body_style),
         Paragraph("Serves as the theoretical foundation for our DAG node lifecycle and variable kill semantics.", table_body_style)],
        [Paragraph("Implementation", table_body_style),
         Paragraph("D. Watson (2017). <i>A Practical Approach to Compiler Construction</i>.", table_body_style),
         Paragraph("Mapping intermediate representations into efficient object-oriented structures.", table_body_style),
         Paragraph("Guides our modular C++ design separating parser, optimizer, visualizer, and code generator.", table_body_style)],
        [Paragraph("Algorithm Speed", table_body_style),
         Paragraph("S. Muchnick (1997). <i>Advanced Compiler Design and Implementation</i>.", table_body_style),
         Paragraph("Using Local Value Numbering hash signatures for constant-time redundancy checking.", table_body_style),
         Paragraph("Provides the foundation for our O(1) hash map signature lookup and commutative canonicalization.", table_body_style)],
        [Paragraph("Simplifications", table_body_style),
         Paragraph("K. Cooper, L. Torczon (2011). <i>Engineering a Compiler</i>.", table_body_style),
         Paragraph("Static constant folding and algebraic identity reduction.", table_body_style),
         Paragraph("Guides our constant evaluation engine and strength reduction identity rules.", table_body_style)]
    ]
    lit_table = Table(lit_data, colWidths=[70, 145, 145, 145])
    lit_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#f1f5f9")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#94a3b8")),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(lit_table)
    story.append(Spacer(1, 8))

    story.append(Paragraph("4. System Design & Architecture", h1_style))
    story.append(Paragraph("<b>Optimization Pipeline Architecture:</b>", h2_style))
    story.append(Paragraph(
        "<b>Raw TAC File (.tac)</b> &rarr; <b>TAC Parser & Tokenizer</b> &rarr; "
        "<b>DAG Optimization Engine (Value Numbering, Commutative Normalizer, Constant Folder, Identity Reducer, Variable Kill Manager)</b> &rarr; "
        "<b>Optimized Code Generator</b> &rarr; <b>Optimized Output File (.tac) & Graphviz Visualizer (.dot)</b>",
        body_style
    ))
    story.append(Spacer(1, 4))

    story.append(Paragraph("<b>Modular Codebase Organization:</b>", h2_style))
    struct_data = [
        [Paragraph("<b>Directory / File</b>", table_header_style), Paragraph("<b>Component Purpose & Responsibilities</b>", table_header_style)],
        [Paragraph("include/Instruction.h", code_style), Paragraph("Defines InstructionType enum (BINARY, UNARY, COPY, CONSTANT) and TAC Instruction struct.", table_body_style)],
        [Paragraph("include/DAGNode.h", code_style), Paragraph("Defines DAGNode struct, ProcessedStep record, and OptimizationStats counters.", table_body_style)],
        [Paragraph("include/Utils.h, src/Utils.cpp", code_style), Paragraph("String trimming, number parsing/formatting, and commutative operator identification.", table_body_style)],
        [Paragraph("include/TACParser.h, src/TACParser.cpp", code_style), Paragraph("Parses TAC text files, strips comments (#, //), and tokenizes LHS and RHS expressions.", table_body_style)],
        [Paragraph("include/DAGOptimizer.h, src/DAGOptimizer.cpp", code_style), Paragraph("Core engine: Value numbering, commutative CSE, reassignment kill, constant folding, and code emission.", table_body_style)],
        [Paragraph("include/Visualizer.h, src/Visualizer.cpp", code_style), Paragraph("Generates Graphviz .dot visual DAG diagrams and formatted terminal comparison tables.", table_body_style)],
        [Paragraph("include/BenchmarkRunner.h, src/BenchmarkRunner.cpp", code_style), Paragraph("Encapsulates automated execution and performance metrics reporting across benchmarks.", table_body_style)],
        [Paragraph("src/main.cpp", code_style), Paragraph("Application entry point supporting CLI argument processing and full interactive UI menu.", table_body_style)],
        [Paragraph("Makefile, run_benchmarks.ps1, run_tests.bat", code_style), Paragraph("Build automation scripts supporting single-command compile and testing.", table_body_style)]
    ]
    struct_table = Table(struct_data, colWidths=[155, 350])
    struct_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#f1f5f9")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#94a3b8")),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 2.5),
        ('BOTTOMPADDING', (0,0), (-1,-1), 2.5),
    ]))
    story.append(struct_table)
    story.append(PageBreak())

    # =========================================================================
    # PAGE 4: LOCAL OPTIMIZATIONS & CORE MECHANISMS
    # =========================================================================
    story.append(Paragraph("5. Local Optimizations Breakdown (Review 2 Comprehensive)", h1_style))
    opt_data = [
        [Paragraph("<b>Optimization Technique</b>", table_header_style), Paragraph("<b>Mechanism & Description</b>", table_header_style), Paragraph("<b>Before vs After Transformation</b>", table_header_style)],
        [Paragraph("<b>Common Subexpression Elimination (CSE)</b>", table_body_style),
         Paragraph("Detects identical operations using signature hashing and reuses the existing primary variable.", table_body_style),
         Paragraph("[Before] t1 = a * b; t2 = a * b<br/>[After] t1 = a * b; t2 = t1", code_style)],
        [Paragraph("<b>Commutative Expression Matching</b>", table_body_style),
         Paragraph("Canonicalizes operands for commutative operators (+, *, &, |, ^, ==, !=) by ordering node IDs: op(min, max).", table_body_style),
         Paragraph("[Before] t1 = a + b; t2 = b + a<br/>[After] t1 = a + b; t2 = t1", code_style)],
        [Paragraph("<b>Variable Reassignment & Node Kill</b>", table_body_style),
         Paragraph("Dragon Book rule: when a variable is reassigned, its label is detached from the old node, preventing stale aliasing.", table_body_style),
         Paragraph("[Before] a = b+c; t1 = a-d; a = 10; t2 = a-d<br/>[After] a = b+c; t1 = a-d; a = 10; t2 = 10-d", code_style)],
        [Paragraph("<b>Compile-Time Constant Folding</b>", table_body_style),
         Paragraph("Statically evaluates arithmetic on constant operands and creates a folded constant leaf node.", table_body_style),
         Paragraph("[Before] t1 = 12 + 8; t2 = t1 * 2<br/>[After] t1 = 20; t2 = 40", code_style)],
        [Paragraph("<b>Algebraic Identity Simplification</b>", table_body_style),
         Paragraph("Applies mathematical reduction rules: x+0=x, x-0=x, x-x=0, x*1=x, x*0=0, x/1=x, x/x=1.", table_body_style),
         Paragraph("[Before] t3 = a * 1; t5 = c * 0; t6 = d - d<br/>[After] t3 = a; t5 = 0; t6 = 0", code_style)],
        [Paragraph("<b>Redundant Leaf Prevention & Copy Propagation</b>", table_body_style),
         Paragraph("Reuses existing symbol table nodes for duplicate operands and propagates copy aliases directly.", table_body_style),
         Paragraph("[Before] t1 = a + b; t2 = a - c; x = y<br/>[After] Both share leaf 'a'; x aliases 'y'", code_style)]
    ]
    opt_table = Table(opt_data, colWidths=[120, 205, 180])
    opt_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#f1f5f9")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#94a3b8")),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(opt_table)
    story.append(Spacer(1, 8))

    story.append(Paragraph("6. Core Mechanisms & Algorithms", h1_style))
    story.append(Paragraph("<b>A. O(1) Value Numbering with Commutative Sorting:</b>", h2_style))
    story.append(Paragraph(
        "Instead of scanning previously processed instructions in O(N^2) time, our DAGOptimizer computes a string signature "
        "for each operation: <code>sig = op + '(' + to_string(leftId) + ',' + to_string(rightId) + ')'</code>. "
        "If <code>isCommutative(op)</code> is true and <code>leftId > rightId</code>, the IDs are swapped. "
        "A hash map lookup (<code>valueNumbering[sig]</code>) verifies uniqueness in O(1) average time.",
        body_style
    ))
    story.append(Spacer(1, 3))

    story.append(Paragraph("<b>B. Dragon Book Node Kill & Label Detach Semantics:</b>", h2_style))
    story.append(Paragraph(
        "When variable <code>x</code> is updated via an instruction: "
        "(1) <code>detachVariable(x)</code> checks if <code>x</code> already exists in <code>varToNode</code>, "
        "removing <code>x</code> from the previous node's active <code>labels</code> vector. "
        "(2) <code>attachVariable(x, targetNodeId)</code> attaches <code>x</code> to the new node and updates "
        "<code>varToNode[x] = targetNodeId</code>. This ensures subsequent instructions reference the updated value.",
        body_style
    ))
    story.append(Spacer(1, 3))

    story.append(Paragraph("<b>C. Chronological Replay Code Generation:</b>", h2_style))
    story.append(Paragraph(
        "The code generator replays each processed step in execution order. If an instruction created a new computation, "
        "it emits the operation with current active representative variables. If it matched an existing node (CSE), "
        "it emits a clean copy assignment (e.g., <code>t2 = t1</code>), perfectly preserving downstream variable dependencies.",
        body_style
    ))
    story.append(PageBreak())

    # =========================================================================
    # PAGE 5: IMPLEMENTATION STATUS & VERIFIED TERMINAL OUTPUT
    # =========================================================================
    story.append(Paragraph("7. Implementation Status (75% Milestone Achieved)", h1_style))
    story.append(Paragraph("• <b>Modular C++ Architecture:</b> Full separation of concerns across 7 header files and 6 source files.", bullet_style))
    story.append(Paragraph("• <b>Robust TAC File I/O:</b> Dynamically parses files, strips whitespace/comments, and handles constants and unary operations.", bullet_style))
    story.append(Paragraph("• <b>Full Optimization Suite:</b> CSE, commutative equivalence, node kill, constant folding, and algebraic reductions fully operational.", bullet_style))
    story.append(Paragraph("• <b>Code Generator & Graphviz Visualizer:</b> Generates optimized .tac files and exports visual .dot graph files.", bullet_style))
    story.append(Paragraph("• <b>Automated Build & Test Infrastructure:</b> Tested via GNU Makefile, PowerShell, and Windows batch runners.", bullet_style))
    story.append(Spacer(1, 4))

    story.append(Paragraph("<b>Verified System Execution Output (TestCase 1 - Standard CSE):</b>", h2_style))
    
    terminal_html = (
        "<font color='#38bdf8'><b>[INFO] Processing TAC file: samples/sample1_readme.tac</b></font><br/>"
        "<font color='#94a3b8'>--- Optimization Pipeline Audit Log ---</font><br/>"
        "  1. <font color='#4ade80'>CSE:</font> 't2 = a * b' maps to existing Node 2 [Represented by t1]<br/>"
        "  2. <font color='#4ade80'>CSE:</font> 't5 = c + d' maps to existing Node 5 [Represented by t3]<br/>"
        "  3. <font color='#4ade80'>CSE:</font> 't6 = t5 - t2' maps to existing Node 6 [Represented by t4]<br/>"
        "<font color='#94a3b8'>---------------------------------------</font><br/>"
        "<br/>"
        "<font color='#fbbf24'>===================================================================</font><br/>"
        "<font color='#fbbf24'>                      CONSTRUCTED DAG NODES                        </font><br/>"
        "<font color='#fbbf24'>===================================================================</font><br/>"
        "ID      Op/Val      Left      Right     Primary Var   Active Labels<br/>"
        "-------------------------------------------------------------------<br/>"
        "0       a           -         -         a             a<br/>"
        "1       b           -         -         b             b<br/>"
        "2       *           0         1         t1            t1 t2<br/>"
        "3       c           -         -         c             c<br/>"
        "4       d           -         -         d             d<br/>"
        "5       +           3         4         t3            t3 t5<br/>"
        "6       -           5         2         t4            t4 t6<br/>"
        "-------------------------------------------------------------------<br/>"
        "<br/>"
        "<font color='#38bdf8'>=======================================================================</font><br/>"
        "<font color='#38bdf8'>             BEFORE vs AFTER OPTIMIZATION COMPARISON                  </font><br/>"
        "<font color='#38bdf8'>=======================================================================</font><br/>"
        "<b>Unoptimized Input (Before)          | Optimized TAC (After)</b><br/>"
        "------------------------------------+----------------------------------<br/>"
        "t1 = a * b                          | t1 = a * b<br/>"
        "t2 = a * b                          | <font color='#4ade80'>t2 = t1</font><br/>"
        "t3 = c + d                          | t3 = c + d<br/>"
        "t4 = t3 - t1                        | t4 = t3 - t1<br/>"
        "t5 = c + d                          | <font color='#4ade80'>t5 = t3</font><br/>"
        "t6 = t5 - t2                        | <font color='#4ade80'>t6 = t4</font><br/>"
        "=======================================================================<br/>"
        "<br/>"
        "<font color='#a78bfa'>=======================================================================</font><br/>"
        "<font color='#a78bfa'>               EMPIRICAL PERFORMANCE METRICS SUMMARY                  </font><br/>"
        "<font color='#a78bfa'>=======================================================================</font><br/>"
        " - Total Original Instructions        : 6<br/>"
        " - Common Subexpressions Eliminated (CSE): 3<br/>"
        " - Commutative Expressions Matched    : 0<br/>"
        " - Constant Foldings Applied           : 0<br/>"
        " - Algebraic Identities Simplified    : 0<br/>"
        " - <font color='#4ade80'><b>Total Redundant Computations Cut   : 3 (50.0% Reduction)</b></font><br/>"
        "=======================================================================<br/>"
        "<font color='#4ade80'>[SUCCESS] Wrote optimized TAC to: optimized_output.tac</font><br/>"
        "<font color='#4ade80'>[SUCCESS] Exported DAG visualization to Graphviz file: dag_output.dot</font>"
    )

    term_table = Table([[Paragraph(terminal_html, terminal_code_style)]], colWidths=[505])
    term_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#0f172a")),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor("#334155")),
        ('TOPPADDING', (0,0), (-1,-1), 6),
        ('BOTTOMPADDING', (0,0), (-1,-1), 6),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
        ('RIGHTPADDING', (0,0), (-1,-1), 8),
    ]))
    story.append(term_table)
    story.append(PageBreak())

    # =========================================================================
    # PAGE 6: BENCHMARKS, REMAINING WORK & CONCLUSION
    # =========================================================================
    story.append(Paragraph("8. Empirical Performance Analysis & Benchmark Evaluation", h1_style))
    story.append(Paragraph(
        "To empirically validate the optimizer's correctness and performance gains, a suite of 5 diverse benchmark test cases "
        "was created and evaluated across all optimization passes:",
        body_style
    ))
    story.append(Spacer(1, 2))

    bm_data = [
        [Paragraph("<b>Benchmark</b>", table_header_style), Paragraph("<b>Test File</b>", table_header_style), Paragraph("<b>Initial</b>", table_header_style), Paragraph("<b>Optimized</b>", table_header_style), Paragraph("<b>Cut</b>", table_header_style), Paragraph("<b>Key Optimizations Triggered</b>", table_header_style)],
        [Paragraph("Test 1: Readme CSE", table_body_style), Paragraph("sample1_readme.tac", code_style), Paragraph("6", table_body_style), Paragraph("6", table_body_style), Paragraph("<b>3 (50%)</b>", table_body_style), Paragraph("Standard CSE & Transitive Redundancy", table_body_style)],
        [Paragraph("Test 2: Commutative", table_body_style), Paragraph("sample2_commutative.tac", code_style), Paragraph("6", table_body_style), Paragraph("6", table_body_style), Paragraph("<b>3 (50%)</b>", table_body_style), Paragraph("Commutative CSE on + and * operators", table_body_style)],
        [Paragraph("Test 3: Reassignment", table_body_style), Paragraph("sample3_reassignment.tac", code_style), Paragraph("6", table_body_style), Paragraph("6", table_body_style), Paragraph("<b>1 (16.7%)</b>", table_body_style), Paragraph("Dragon Book Node Kill & Label Detach", table_body_style)],
        [Paragraph("Test 4: Constant/Identities", table_body_style), Paragraph("sample4_constant_folding.tac", code_style), Paragraph("7", table_body_style), Paragraph("7", table_body_style), Paragraph("<b>7 (100%)</b>", table_body_style), Paragraph("3 Constant Foldings + 4 Algebraic Identities", table_body_style)],
        [Paragraph("Test 5: Matrix Indexing", table_body_style), Paragraph("sample5_matrix_offset.tac", code_style), Paragraph("13", table_body_style), Paragraph("13", table_body_style), Paragraph("<b>6 (46.2%)</b>", table_body_style), Paragraph("Repeated Array Offset Address Computations", table_body_style)]
    ]
    bm_table = Table(bm_data, colWidths=[95, 120, 36, 48, 50, 156])
    bm_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#f1f5f9")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#94a3b8")),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(bm_table)
    story.append(Spacer(1, 8))

    story.append(Paragraph("9. Remaining Work for Review 3 (Final 100% Milestone)", h1_style))
    story.append(Paragraph("Having completed 75% of the project scope in Review 2, the remaining 25% for Review 3 will cover:", body_style))
    story.append(Paragraph("• <b>Control Flow Graph (CFG) Construction:</b> Identifying basic block leaders and generating edges for conditional/unconditional jumps.", bullet_style))
    story.append(Paragraph("• <b>Global Dataflow Analysis:</b> Implementing Live Variable Analysis and Available Expressions across block boundaries.", bullet_style))
    story.append(Paragraph("• <b>Global Dead Code Elimination:</b> Eliminating dead temporaries across the entire function CFG.", bullet_style))
    story.append(Paragraph("• <b>Target Code Generation:</b> Translating optimized TAC into assembly instructions (MIPS or x86).", bullet_style))
    story.append(Spacer(1, 8))

    story.append(Paragraph("10. Conclusion", h1_style))
    story.append(Paragraph(
        "For Review 2 (75% milestone), the project has transitioned from a hardcoded concept into a fully operational, "
        "production-grade basic block optimizer. The system seamlessly parses raw Three-Address Code files, constructs "
        "the DAG using an O(1) hash-based Value Numbering algorithm, eliminates common subexpressions (including commutative "
        "variants), correctly detaches reassigned variables, evaluates compile-time constants, reduces algebraic identities, "
        "and reconstructs shortened, semantically equivalent TAC while providing visual Graphviz DAG diagrams. "
        "Empirical benchmarks confirm up to 50% reduction in redundant computations, establishing a robust foundation for Review 3.",
        body_style
    ))

    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"[SUCCESS] Successfully generated {pdf_filename}!")

if __name__ == '__main__':
    create_report()
