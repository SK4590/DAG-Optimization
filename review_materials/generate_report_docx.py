import docx
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_ALIGN_VERTICAL
from docx.oxml import parse_xml, OxmlElement
from docx.oxml.ns import nsdecls, qn

def set_cell_background(cell, hex_color):
    shading_elm = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{hex_color}"/>')
    cell._tc.get_or_add_tcPr().append(shading_elm)

def set_cell_margins(cell, top=100, bottom=100, left=150, right=150):
    tcPr = cell._tc.get_or_add_tcPr()
    tcMar = OxmlElement('w:tcMar')
    for m, val in [('top', top), ('bottom', bottom), ('left', left), ('right', right)]:
        node = OxmlElement(f'w:{m}')
        node.set(qn('w:w'), str(val))
        node.set(qn('w:type'), 'dxa')
        tcMar.append(node)
    tcPr.append(tcMar)

def set_table_borders(table, color="CBD5E1", sz="4", val="single"):
    tblPr = table._tbl.tblPr
    borders = parse_xml(
        f'<w:tblBorders {nsdecls("w")}>'
        f'<w:top w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:left w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:bottom w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:right w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:insideH w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:insideV w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'</w:tblBorders>'
    )
    tblPr.append(borders)

def build_docx():
    doc = docx.Document()
    
    # Page Setup - A4
    for section in doc.sections:
        section.page_width = Inches(8.27)
        section.page_height = Inches(11.69)
        section.top_margin = Inches(0.8)
        section.bottom_margin = Inches(0.8)
        section.left_margin = Inches(0.8)
        section.right_margin = Inches(0.8)

        # Header and Footer
        header = section.header
        hp = header.paragraphs[0]
        hp.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        hrun = hp.add_run("Compiler Design Lab | Project Review 2 Report (75% Milestone)")
        hrun.font.name = "Calibri"
        hrun.font.size = Pt(8.5)
        hrun.font.color.rgb = RGBColor(148, 163, 184)

        footer = section.footer
        fp = footer.paragraphs[0]
        fp.alignment = WD_ALIGN_PARAGRAPH.CENTER
        frun = fp.add_run("Page Reference - Review 2 Deliverable")
        frun.font.name = "Calibri"
        frun.font.size = Pt(8.5)
        frun.font.color.rgb = RGBColor(148, 163, 184)

    # Styles
    normal_style = doc.styles['Normal']
    normal_style.font.name = 'Calibri'
    normal_style.font.size = Pt(10)
    normal_style.font.color.rgb = RGBColor(51, 65, 85)

    # =========================================================================
    # PAGE 1: TITLE PAGE
    # =========================================================================
    p_pre = doc.add_paragraph()
    p_pre.paragraph_format.space_before = Pt(120)

    p_title = doc.add_paragraph()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_title = p_title.add_run("Compiler Design Lab – Project Review 2\nReport")
    r_title.font.name = "Calibri"
    r_title.font.size = Pt(22)
    r_title.font.bold = True
    r_title.font.color.rgb = RGBColor(15, 23, 42)
    p_title.paragraph_format.space_after = Pt(40)

    p_proj_lbl = doc.add_paragraph()
    p_proj_lbl.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_lbl = p_proj_lbl.add_run("Project Title:")
    r_lbl.font.name = "Calibri"
    r_lbl.font.size = Pt(14)
    r_lbl.font.bold = True
    r_lbl.font.color.rgb = RGBColor(30, 41, 59)
    p_proj_lbl.paragraph_format.space_after = Pt(10)

    p_proj_title = doc.add_paragraph()
    p_proj_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_pt = p_proj_title.add_run("DAG-Based Optimization for Basic Blocks")
    r_pt.font.name = "Calibri"
    r_pt.font.size = Pt(20)
    r_pt.font.bold = True
    r_pt.font.color.rgb = RGBColor(15, 23, 42)
    p_proj_title.paragraph_format.space_after = Pt(25)

    # Milestone Box
    badge_table = doc.add_table(rows=1, cols=1)
    badge_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    c = badge_table.rows[0].cells[0]
    c.width = Inches(5.5)
    set_cell_background(c, "DCFCE7")
    set_cell_margins(c, 120, 120, 200, 200)
    cp = c.paragraphs[0]
    cp.alignment = WD_ALIGN_PARAGRAPH.CENTER
    c_run = cp.add_run("Implementation Progress: 75% Complete (Review 2)")
    c_run.font.name = "Calibri"
    c_run.font.size = Pt(11)
    c_run.font.bold = True
    c_run.font.color.rgb = RGBColor(6, 95, 70)
    
    # Members
    p_team_pre = doc.add_paragraph()
    p_team_pre.paragraph_format.space_before = Pt(80)

    p_team_lbl = doc.add_paragraph()
    p_team_lbl.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_tl = p_team_lbl.add_run("Team Members:")
    r_tl.font.name = "Calibri"
    r_tl.font.size = Pt(13)
    r_tl.font.bold = True
    r_tl.font.color.rgb = RGBColor(15, 23, 42)
    p_team_lbl.paragraph_format.space_after = Pt(12)

    for member in ["• Abhinav Gupta (24BCT0185)", "• Shobit Khanna (24BCT0187)", "• Arnav Goel (24BCT0189)"]:
        pm = doc.add_paragraph()
        pm.alignment = WD_ALIGN_PARAGRAPH.CENTER
        rm = pm.add_run(member)
        rm.font.name = "Calibri"
        rm.font.size = Pt(11.5)
        rm.font.color.rgb = RGBColor(51, 65, 85)
        pm.paragraph_format.space_after = Pt(4)

    doc.add_page_break()

    # Helper function for Section Headings
    def add_h1(text):
        h = doc.add_paragraph()
        h.paragraph_format.space_before = Pt(14)
        h.paragraph_format.space_after = Pt(4)
        h.paragraph_format.keep_with_next = True
        r = h.add_run(text)
        r.font.name = "Calibri"
        r.font.size = Pt(14)
        r.font.bold = True
        r.font.color.rgb = RGBColor(15, 23, 42)
        return h

    def add_h2(text):
        h = doc.add_paragraph()
        h.paragraph_format.space_before = Pt(10)
        h.paragraph_format.space_after = Pt(3)
        h.paragraph_format.keep_with_next = True
        r = h.add_run(text)
        r.font.name = "Calibri"
        r.font.size = Pt(11.5)
        r.font.bold = True
        r.font.color.rgb = RGBColor(30, 41, 59)
        return h

    def add_bullet(text, bold_prefix=""):
        p = doc.add_paragraph(style='List Bullet')
        p.paragraph_format.space_before = Pt(1)
        p.paragraph_format.space_after = Pt(2)
        p.paragraph_format.left_indent = Inches(0.25)
        if bold_prefix:
            rb = p.add_run(bold_prefix)
            rb.font.bold = True
            rb.font.color.rgb = RGBColor(15, 23, 42)
        r = p.add_run(text)
        r.font.color.rgb = RGBColor(51, 65, 85)
        return p

    def add_body(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(2)
        p.paragraph_format.space_after = Pt(4)
        r = p.add_run(text)
        r.font.color.rgb = RGBColor(51, 65, 85)
        return p

    # =========================================================================
    # PAGE 2: PROBLEM STATEMENT & OBJECTIVES
    # =========================================================================
    add_h1("1. Problem Statement")
    add_body(
        "Unoptimized intermediate code, such as standard Three-Address Code (TAC) generated by compiler frontends, "
        "frequently contains redundant computations, duplicate subexpressions, and dead operations. Values are repeatedly "
        "recomputed across a basic block, unnecessarily wasting CPU cycles, memory bandwidth, and execution time. "
        "Our goal is to build an optimizing compiler engine that constructs a Directed Acyclic Graph (DAG) for basic blocks "
        "to automatically detect, fold, simplify, and eliminate these redundancies while synthesizing valid, optimized TAC."
    )

    add_h2("Key Challenges Addressed:")
    add_bullet(" Eliminating duplicate calculations in O(1) time without costly full-graph traversals.", "Constant-Time Redundancy Detection:")
    add_bullet(" Recognizing semantic equivalence for commutative operations (e.g., a + b == b + a).", "Commutative Operations:")
    add_bullet(" Handling Dragon Book node kill and label detachment when variables are overwritten.", "Variable Reassignment & Aliasing:")
    add_bullet(" Folding static arithmetic and propagating constant values through the graph.", "Compile-Time Constant Evaluation:")
    add_bullet(" Identifying mathematical identities (x + 0, x * 1, x * 0, x - x) for strength reduction.", "Algebraic Reductions:")
    add_bullet(" Reconstructing shortened, valid Three-Address Code and Graphviz visualizations from the DAG.", "Code Generation:")

    add_h2("Naive Input vs Optimized Output:")
    
    table_io = doc.add_table(rows=2, cols=3)
    table_io.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_borders(table_io)
    headers_io = ["Unoptimized TAC (Before)", "Optimized TAC (After)", "Optimization Applied"]
    for i, h in enumerate(headers_io):
        c = table_io.rows[0].cells[i]
        set_cell_background(c, "F1F5F9")
        set_cell_margins(c, 80, 80, 100, 100)
        p = c.paragraphs[0]
        r = p.add_run(h)
        r.font.bold = True
        r.font.size = Pt(9.5)
        r.font.color.rgb = RGBColor(15, 23, 42)

    rows_io = [
        ("t1 = a * b\nt2 = a * b\nt3 = c + d\nt4 = t3 - t1\nt5 = c + d\nt6 = t5 - t2",
         "t1 = a * b\nt2 = t1\nt3 = c + d\nt4 = t3 - t1\nt5 = t3\nt6 = t4",
         "Initial Computation\nCSE: Reuses Node t1\nInitial Computation\nComputation\nCSE: Reuses Node t3\nCSE: Transitive Subexpression")
    ]
    for r_idx, (c0, c1, c2) in enumerate(rows_io, start=1):
        for col_idx, text in enumerate([c0, c1, c2]):
            c = table_io.rows[r_idx].cells[col_idx]
            set_cell_margins(c, 80, 80, 100, 100)
            p = c.paragraphs[0]
            r = p.add_run(text)
            r.font.size = Pt(8.5)
            if col_idx < 2:
                r.font.name = "Consolas"
            r.font.color.rgb = RGBColor(30, 41, 59)

    add_h1("2. Objectives")
    add_h2("Review 1 Objectives (25% - Completed Recap):")
    add_bullet("Establish core data structures for DAG nodes and leaf representations.")
    add_bullet("Implement Value Numbering using hash maps for constant-time O(1) duplicate detection.")
    add_bullet("Demonstrate proof-of-concept graph construction on a toy hardcoded 4-instruction basic block.")

    add_h2("Review 2 Objectives (75% - Implemented & Delivered):")
    add_bullet(" Replace hardcoded strings with a robust file parser handling binary, unary, copy, and constants.", "Dynamic TAC File Parser:")
    add_bullet(" Restructure into production-grade C++ headers (include/) and implementations (src/).", "Modular Architectural Refactoring:")
    add_bullet(" Canonicalize commutative operators (+, *, &, |, ^, ==, !=) using sorted operand IDs.", "Commutative CSE:")
    add_bullet(" Implement strict Dragon Book label detachment when variables are overwritten.", "Node Kill & Reassignment:")
    add_bullet(" Static compile-time evaluation and neutral/annihilator reduction.", "Constant Folding & Algebraic Identities:")
    add_bullet(" Traverse and replay DAG to emit clean, shortened TAC files.", "Optimized TAC Code Generator:")
    add_bullet(" Generate .dot graph files for graphical DAG rendering.", "Visual Graphviz Exporter:")
    add_bullet(" Validate against 5 comprehensive benchmark test suites with detailed metrics.", "Empirical Benchmarks:")

    doc.add_page_break()

    # =========================================================================
    # PAGE 3: LITERATURE SURVEY & SYSTEM DESIGN
    # =========================================================================
    add_h1("3. Literature Survey")
    
    lit_table = doc.add_table(rows=5, cols=4)
    lit_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_borders(lit_table)
    lit_headers = ["Theme", "Books / Papers", "Key Idea", "Relevance to this Project"]
    for i, h in enumerate(lit_headers):
        c = lit_table.rows[0].cells[i]
        set_cell_background(c, "F1F5F9")
        set_cell_margins(c, 80, 80, 100, 100)
        p = c.paragraphs[0]
        r = p.add_run(h)
        r.font.bold = True
        r.font.size = Pt(9)
        r.font.color.rgb = RGBColor(15, 23, 42)

    lit_data = [
        ("Foundations", "A. V. Aho, M. S. Lam, R. Sethi, J. D. Ullman (2007). Compilers: Principles, Techniques, & Tools.",
         "Constructing DAGs for basic blocks; variable reassignment and label kill rules.",
         "Serves as the theoretical foundation for our DAG node lifecycle and variable kill semantics."),
        ("Implementation", "D. Watson (2017). A Practical Approach to Compiler Construction.",
         "Mapping intermediate representations into efficient object-oriented structures.",
         "Guides our modular C++ design separating parser, optimizer, visualizer, and code generator."),
        ("Algorithm Speed", "S. Muchnick (1997). Advanced Compiler Design and Implementation.",
         "Using Local Value Numbering hash signatures for constant-time redundancy checking.",
         "Provides the foundation for our O(1) hash map signature lookup and commutative canonicalization."),
        ("Simplifications", "K. Cooper, L. Torczon (2011). Engineering a Compiler.",
         "Static constant folding and algebraic identity reduction.",
         "Guides our constant evaluation engine and strength reduction identity rules.")
    ]
    for r_idx, row in enumerate(lit_data, start=1):
        for c_idx, text in enumerate(row):
            c = lit_table.rows[r_idx].cells[c_idx]
            set_cell_margins(c, 60, 60, 80, 80)
            p = c.paragraphs[0]
            r = p.add_run(text)
            r.font.size = Pt(8.5)
            r.font.color.rgb = RGBColor(51, 65, 85)

    add_h1("4. System Design & Architecture")
    add_h2("Optimization Pipeline Architecture:")
    add_body(
        "Raw TAC File (.tac) → TAC Parser & Tokenizer → "
        "DAG Optimization Engine (Value Numbering, Commutative Normalizer, Constant Folder, Identity Reducer, Variable Kill Manager) → "
        "Optimized Code Generator → Optimized Output File (.tac) & Graphviz Visualizer (.dot)"
    )

    add_h2("Modular Codebase Organization:")
    
    mod_table = doc.add_table(rows=10, cols=2)
    mod_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_borders(mod_table)
    for i, h in enumerate(["Directory / File", "Component Purpose & Responsibilities"]):
        c = mod_table.rows[0].cells[i]
        set_cell_background(c, "F1F5F9")
        set_cell_margins(c, 60, 60, 80, 80)
        p = c.paragraphs[0]
        r = p.add_run(h)
        r.font.bold = True
        r.font.size = Pt(9)
        r.font.color.rgb = RGBColor(15, 23, 42)

    mod_data = [
        ("include/Instruction.h", "Defines InstructionType enum (BINARY, UNARY, COPY, CONSTANT) and TAC Instruction struct."),
        ("include/DAGNode.h", "Defines DAGNode struct, ProcessedStep record, and OptimizationStats counters."),
        ("include/Utils.h, src/Utils.cpp", "String trimming, number parsing/formatting, and commutative operator identification."),
        ("include/TACParser.h, src/TACParser.cpp", "Parses TAC text files, strips comments (#, //), and tokenizes LHS and RHS expressions."),
        ("include/DAGOptimizer.h, src/DAGOptimizer.cpp", "Core engine: Value numbering, commutative CSE, reassignment kill, constant folding, and code emission."),
        ("include/Visualizer.h, src/Visualizer.cpp", "Generates Graphviz .dot visual DAG diagrams and formatted terminal comparison tables."),
        ("include/BenchmarkRunner.h, src/BenchmarkRunner.cpp", "Encapsulates automated execution and performance metrics reporting across benchmarks."),
        ("src/main.cpp", "Application entry point supporting CLI argument processing and full interactive UI menu."),
        ("Makefile, run_benchmarks.ps1, run_tests.bat", "Build automation scripts supporting single-command compile and testing.")
    ]
    for r_idx, (f, desc) in enumerate(mod_data, start=1):
        c0 = mod_table.rows[r_idx].cells[0]
        c1 = mod_table.rows[r_idx].cells[1]
        set_cell_margins(c0, 50, 50, 70, 70)
        set_cell_margins(c1, 50, 50, 70, 70)
        p0 = c0.paragraphs[0]
        r0 = p0.add_run(f)
        r0.font.name = "Consolas"
        r0.font.size = Pt(8.5)
        p1 = c1.paragraphs[0]
        r1 = p1.add_run(desc)
        r1.font.size = Pt(8.5)

    doc.add_page_break()

    # =========================================================================
    # PAGE 4: LOCAL OPTIMIZATIONS & CORE MECHANISMS
    # =========================================================================
    add_h1("5. Local Optimizations Breakdown (Review 2 Comprehensive)")
    
    opt_table = doc.add_table(rows=7, cols=3)
    opt_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_borders(opt_table)
    for i, h in enumerate(["Optimization Technique", "Mechanism & Description", "Before vs After Transformation"]):
        c = opt_table.rows[0].cells[i]
        set_cell_background(c, "F1F5F9")
        set_cell_margins(c, 70, 70, 90, 90)
        p = c.paragraphs[0]
        r = p.add_run(h)
        r.font.bold = True
        r.font.size = Pt(9)
        r.font.color.rgb = RGBColor(15, 23, 42)

    opt_rows = [
        ("Common Subexpression Elimination (CSE)", "Detects identical operations using signature hashing and reuses the existing primary variable.", "[Before] t1 = a * b; t2 = a * b\n[After] t1 = a * b; t2 = t1"),
        ("Commutative Expression Matching", "Canonicalizes operands for commutative operators (+, *, &, |, ^, ==, !=) by ordering node IDs: op(min, max).", "[Before] t1 = a + b; t2 = b + a\n[After] t1 = a + b; t2 = t1"),
        ("Variable Reassignment & Node Kill", "Dragon Book rule: when a variable is reassigned, its label is detached from the old node, preventing stale aliasing.", "[Before] a = b+c; t1 = a-d; a = 10; t2 = a-d\n[After] a = b+c; t1 = a-d; a = 10; t2 = 10-d"),
        ("Compile-Time Constant Folding", "Statically evaluates arithmetic on constant operands and creates a folded constant leaf node.", "[Before] t1 = 12 + 8; t2 = t1 * 2\n[After] t1 = 20; t2 = 40"),
        ("Algebraic Identity Simplification", "Applies mathematical reduction rules: x+0=x, x-0=x, x-x=0, x*1=x, x*0=0, x/1=x, x/x=1.", "[Before] t3 = a * 1; t5 = c * 0; t6 = d - d\n[After] t3 = a; t5 = 0; t6 = 0"),
        ("Redundant Leaf Prevention & Copy Propagation", "Reuses existing symbol table nodes for duplicate operands and propagates copy aliases directly.", "[Before] t1 = a + b; t2 = a - c; x = y\n[After] Both share leaf 'a'; x aliases 'y'")
    ]
    for r_idx, (t_name, t_desc, t_code) in enumerate(opt_rows, start=1):
        c0 = opt_table.rows[r_idx].cells[0]
        c1 = opt_table.rows[r_idx].cells[1]
        c2 = opt_table.rows[r_idx].cells[2]
        set_cell_margins(c0, 60, 60, 80, 80)
        set_cell_margins(c1, 60, 60, 80, 80)
        set_cell_margins(c2, 60, 60, 80, 80)
        p0 = c0.paragraphs[0]
        r0 = p0.add_run(t_name)
        r0.font.bold = True
        r0.font.size = Pt(8.5)
        p1 = c1.paragraphs[0]
        r1 = p1.add_run(t_desc)
        r1.font.size = Pt(8.5)
        p2 = c2.paragraphs[0]
        r2 = p2.add_run(t_code)
        r2.font.name = "Consolas"
        r2.font.size = Pt(8)

    add_h1("6. Core Mechanisms & Algorithms")
    add_h2("A. O(1) Value Numbering with Commutative Sorting:")
    add_body(
        "Instead of scanning previously processed instructions in O(N^2) time, our DAGOptimizer computes a string signature "
        "for each operation: sig = op + '(' + to_string(leftId) + ',' + to_string(rightId) + ')'. If isCommutative(op) is true and leftId > rightId, the IDs are "
        "swapped. A hash map lookup (valueNumbering[sig]) verifies uniqueness in O(1) average time."
    )

    add_h2("B. Dragon Book Node Kill & Label Detach Semantics:")
    add_body(
        "When variable x is updated via an instruction: (1) detachVariable(x) checks if x already exists in varToNode, "
        "removing x from the previous node's active labels vector. (2) attachVariable(x, targetNodeId) attaches x to the new node and updates "
        "varToNode[x] = targetNodeId. This ensures subsequent instructions reference the updated value."
    )

    add_h2("C. Chronological Replay Code Generation:")
    add_body(
        "The code generator replays each processed step in execution order. If an instruction created a new computation, "
        "it emits the operation with current active representative variables. If it matched an existing node (CSE), "
        "it emits a clean copy assignment (e.g., t2 = t1), perfectly preserving downstream variable dependencies."
    )

    doc.add_page_break()

    # =========================================================================
    # PAGE 5: IMPLEMENTATION STATUS & VERIFIED TERMINAL OUTPUT
    # =========================================================================
    add_h1("7. Implementation Status (75% Milestone Achieved)")
    add_bullet("Full separation of concerns across 7 header files and 6 source files.", "Modular C++ Architecture: ")
    add_bullet("Dynamically parses files, strips whitespace/comments, and handles constants and unary operations.", "Robust TAC File I/O: ")
    add_bullet("CSE, commutative equivalence, node kill, constant folding, and algebraic reductions fully operational.", "Full Optimization Suite: ")
    add_bullet("Generates optimized .tac files and exports visual .dot graph files.", "Code Generator & Graphviz Visualizer: ")
    add_bullet("Tested via GNU Makefile, PowerShell, and Windows batch runners.", "Automated Build & Test Infrastructure: ")

    add_h2("Verified System Execution Output (TestCase 1 - Standard CSE):")
    
    term_box = doc.add_table(rows=1, cols=1)
    term_box.alignment = WD_TABLE_ALIGNMENT.CENTER
    c_term = term_box.rows[0].cells[0]
    set_cell_background(c_term, "0F172A")
    set_cell_margins(c_term, 100, 100, 120, 120)
    
    term_text = (
        "[INFO] Processing TAC file: samples/sample1_readme.tac\n\n"
        "--- Optimization Pipeline Audit Log ---\n"
        "  1. CSE: 't2 = a * b' maps to existing Node 2 [Represented by t1]\n"
        "  2. CSE: 't5 = c + d' maps to existing Node 5 [Represented by t3]\n"
        "  3. CSE: 't6 = t5 - t2' maps to existing Node 6 [Represented by t4]\n"
        "---------------------------------------\n\n"
        "===================================================================\n"
        "                      CONSTRUCTED DAG NODES                        \n"
        "===================================================================\n"
        "ID      Op/Val      Left      Right     Primary Var   Active Labels\n"
        "-------------------------------------------------------------------\n"
        "0       a           -         -         a             a\n"
        "1       b           -         -         b             b\n"
        "2       *           0         1         t1            t1 t2\n"
        "3       c           -         -         c             c\n"
        "4       d           -         -         d             d\n"
        "5       +           3         4         t3            t3 t5\n"
        "6       -           5         2         t4            t4 t6\n"
        "-------------------------------------------------------------------\n\n"
        "=======================================================================\n"
        "             BEFORE vs AFTER OPTIMIZATION COMPARISON                  \n"
        "=======================================================================\n"
        "Unoptimized Input (Before)          | Optimized TAC (After)\n"
        "------------------------------------+----------------------------------\n"
        "t1 = a * b                          | t1 = a * b\n"
        "t2 = a * b                          | t2 = t1\n"
        "t3 = c + d                          | t3 = c + d\n"
        "t4 = t3 - t1                        | t4 = t3 - t1\n"
        "t5 = c + d                          | t5 = t3\n"
        "t6 = t5 - t2                        | t6 = t4\n"
        "=======================================================================\n\n"
        "=======================================================================\n"
        "               EMPIRICAL PERFORMANCE METRICS SUMMARY                  \n"
        "=======================================================================\n"
        " - Total Original Instructions        : 6\n"
        " - Common Subexpressions Eliminated (CSE): 3\n"
        " - Commutative Expressions Matched    : 0\n"
        " - Constant Foldings Applied           : 0\n"
        " - Algebraic Identities Simplified    : 0\n"
        " - Total Redundant Computations Cut   : 3 (50.0% Reduction)\n"
        "=======================================================================\n"
        "[SUCCESS] Wrote optimized TAC to: optimized_output.tac\n"
        "[SUCCESS] Exported DAG visualization to Graphviz file: dag_output.dot"
    )
    p_term = c_term.paragraphs[0]
    r_term = p_term.add_run(term_text)
    r_term.font.name = "Consolas"
    r_term.font.size = Pt(7)
    r_term.font.color.rgb = RGBColor(248, 250, 252)

    doc.add_page_break()

    # =========================================================================
    # PAGE 6: BENCHMARKS, REMAINING WORK & CONCLUSION
    # =========================================================================
    add_h1("8. Empirical Performance Analysis & Benchmark Evaluation")
    add_body(
        "To empirically validate the optimizer's correctness and performance gains, a suite of 5 diverse benchmark test cases "
        "was created and evaluated across all optimization passes:"
    )

    bm_table = doc.add_table(rows=6, cols=6)
    bm_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_borders(bm_table)
    bm_headers = ["Benchmark", "Test File", "Initial", "Optimized", "Cut", "Key Optimizations Triggered"]
    for i, h in enumerate(bm_headers):
        c = bm_table.rows[0].cells[i]
        set_cell_background(c, "F1F5F9")
        set_cell_margins(c, 70, 70, 90, 90)
        p = c.paragraphs[0]
        r = p.add_run(h)
        r.font.bold = True
        r.font.size = Pt(8.5)
        r.font.color.rgb = RGBColor(15, 23, 42)

    bm_rows = [
        ("Test 1: Readme CSE", "sample1_readme.tac", "6", "6", "3 (50%)", "Standard CSE & Transitive Redundancy"),
        ("Test 2: Commutative", "sample2_commutative.tac", "6", "6", "3 (50%)", "Commutative CSE on + and * operators"),
        ("Test 3: Reassignment", "sample3_reassignment.tac", "6", "6", "1 (16.7%)", "Dragon Book Node Kill & Label Detach"),
        ("Test 4: Constant/Identities", "sample4_constant_folding.tac", "7", "7", "7 (100%)", "3 Constant Foldings + 4 Algebraic Identities"),
        ("Test 5: Matrix Indexing", "sample5_matrix_offset.tac", "13", "13", "6 (46.2%)", "Repeated Array Offset Address Computations")
    ]
    for r_idx, row in enumerate(bm_rows, start=1):
        for c_idx, text in enumerate(row):
            c = bm_table.rows[r_idx].cells[c_idx]
            set_cell_margins(c, 60, 60, 80, 80)
            p = c.paragraphs[0]
            r = p.add_run(text)
            r.font.size = Pt(8.5)
            if c_idx == 1:
                r.font.name = "Consolas"
            if c_idx == 4:
                r.font.bold = True
            r.font.color.rgb = RGBColor(51, 65, 85)

    add_h1("9. Remaining Work for Review 3 (Final 100% Milestone)")
    add_body("Having completed 75% of the project scope in Review 2, the remaining 25% for Review 3 will cover:")
    add_bullet("Identifying basic block leaders and generating edges for conditional/unconditional jumps.", "Control Flow Graph (CFG) Construction: ")
    add_bullet("Implementing Live Variable Analysis and Available Expressions across block boundaries.", "Global Dataflow Analysis: ")
    add_bullet("Eliminating dead temporaries across the entire function CFG.", "Global Dead Code Elimination: ")
    add_bullet("Translating optimized TAC into assembly instructions (MIPS or x86).", "Target Code Generation: ")

    add_h1("10. Conclusion")
    add_body(
        "For Review 2 (75% milestone), the project has transitioned from a hardcoded concept into a fully operational, "
        "production-grade basic block optimizer. The system seamlessly parses raw Three-Address Code files, constructs "
        "the DAG using an O(1) hash-based Value Numbering algorithm, eliminates common subexpressions (including commutative "
        "variants), correctly detaches reassigned variables, evaluates compile-time constants, reduces algebraic identities, "
        "and reconstructs shortened, semantically equivalent TAC while providing visual Graphviz DAG diagrams. "
        "Empirical benchmarks confirm up to 50% reduction in redundant computations, establishing a robust foundation for Review 3."
    )

    docx_filename = "CD_LAB_ProjRev2.docx"
    doc.save(docx_filename)
    print(f"[SUCCESS] Successfully generated {docx_filename}!")

if __name__ == '__main__':
    build_docx()
