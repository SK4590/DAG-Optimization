import os
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
from pptx.enum.shapes import MSO_SHAPE

def create_deck():
    prs = Presentation()
    prs.slide_width = Inches(13.333)
    prs.slide_height = Inches(7.5)

    blank_layout = prs.slide_layouts[6] # Blank slide

    # Color palette: Strict Monochrome / Grayscale (Academic, No AI fluff)
    COLOR_BLACK = RGBColor(17, 24, 39)       # #111827
    COLOR_DARK_GRAY = RGBColor(55, 65, 81)   # #374151
    COLOR_MID_GRAY = RGBColor(107, 114, 128) # #6B7280
    COLOR_LIGHT_GRAY = RGBColor(243, 244, 246) # #F3F4F6
    COLOR_BORDER_GRAY = RGBColor(209, 213, 219) # #D1D5DB
    COLOR_WHITE = RGBColor(255, 255, 255)

    def add_header(slide, title_text, subtitle_text="Compiler Design Lab | Project Review 2 (75% Milestone)"):
        # Running top header
        top_box = slide.shapes.add_textbox(Inches(0.8), Inches(0.4), Inches(11.733), Inches(0.4))
        tf_top = top_box.text_frame
        tf_top.word_wrap = True
        tf_top.margin_left = tf_top.margin_right = tf_top.margin_top = tf_top.margin_bottom = 0
        p_sub = tf_top.paragraphs[0]
        p_sub.text = subtitle_text.upper()
        p_sub.font.name = "Calibri"
        p_sub.font.size = Pt(9.5)
        p_sub.font.bold = True
        p_sub.font.color.rgb = COLOR_MID_GRAY

        # Slide Title
        title_box = slide.shapes.add_textbox(Inches(0.8), Inches(0.7), Inches(11.733), Inches(0.6))
        tf_title = title_box.text_frame
        tf_title.word_wrap = True
        tf_title.margin_left = tf_title.margin_right = tf_title.margin_top = tf_title.margin_bottom = 0
        p_title = tf_title.paragraphs[0]
        p_title.text = title_text
        p_title.font.name = "Calibri"
        p_title.font.size = Pt(20)
        p_title.font.bold = True
        p_title.font.color.rgb = COLOR_BLACK

        # Divider line
        line = slide.shapes.add_shape(
            MSO_SHAPE.RECTANGLE, Inches(0.8), Inches(1.35), Inches(11.733), Inches(0.015)
        )
        line.fill.solid()
        line.fill.fore_color.rgb = COLOR_BORDER_GRAY
        line.line.color.rgb = COLOR_BORDER_GRAY

    # =========================================================================
    # SLIDE 1: TITLE SLIDE
    # =========================================================================
    s1 = prs.slides.add_slide(blank_layout)

    # Top border line
    t_line = s1.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(1.0), Inches(1.2), Inches(11.333), Inches(0.02))
    t_line.fill.solid()
    t_line.fill.fore_color.rgb = COLOR_BLACK
    t_line.line.color.rgb = COLOR_BLACK

    tb = s1.shapes.add_textbox(Inches(1.0), Inches(1.5), Inches(11.333), Inches(2.2))
    tf = tb.text_frame
    tf.word_wrap = True
    
    p0 = tf.paragraphs[0]
    p0.text = "COMPILER DESIGN LAB — PROJECT REVIEW 2"
    p0.font.name = "Calibri"
    p0.font.size = Pt(13)
    p0.font.bold = True
    p0.font.color.rgb = COLOR_MID_GRAY
    p0.space_after = Pt(12)

    p1 = tf.add_paragraph()
    p1.text = "DAG-Based Optimization for Basic Blocks"
    p1.font.name = "Calibri"
    p1.font.size = Pt(32)
    p1.font.bold = True
    p1.font.color.rgb = COLOR_BLACK
    p1.space_after = Pt(8)

    p2 = tf.add_paragraph()
    p2.text = "Implementation Milestone: 75% Completed"
    p2.font.name = "Calibri"
    p2.font.size = Pt(14)
    p2.font.color.rgb = COLOR_DARK_GRAY

    b_line = s1.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(1.0), Inches(4.0), Inches(11.333), Inches(0.015))
    b_line.fill.solid()
    b_line.fill.fore_color.rgb = COLOR_BORDER_GRAY
    b_line.line.color.rgb = COLOR_BORDER_GRAY

    tb_team = s1.shapes.add_textbox(Inches(1.0), Inches(4.3), Inches(11.333), Inches(2.5))
    tf_team = tb_team.text_frame
    tf_team.word_wrap = True

    pt0 = tf_team.paragraphs[0]
    pt0.text = "PROJECT TEAM MEMBERS:"
    pt0.font.name = "Calibri"
    pt0.font.size = Pt(11)
    pt0.font.bold = True
    pt0.font.color.rgb = COLOR_BLACK
    pt0.space_after = Pt(8)

    members = [
        "• Abhinav Gupta (24BCT0185)",
        "• Shobit Khanna (24BCT0187)",
        "• Arnav Goel (24BCT0189)"
    ]
    for m in members:
        pm = tf_team.add_paragraph()
        pm.text = m
        pm.font.name = "Calibri"
        pm.font.size = Pt(13)
        pm.font.color.rgb = COLOR_DARK_GRAY
        pm.space_after = Pt(4)

    # =========================================================================
    # SLIDE 2: PROBLEM STATEMENT & MOTIVATION
    # =========================================================================
    s2 = prs.slides.add_slide(blank_layout)
    add_header(s2, "1. Problem Statement & Motivation")

    tb_left = s2.shapes.add_textbox(Inches(0.8), Inches(1.6), Inches(6.0), Inches(5.2))
    tf_l = tb_left.text_frame
    tf_l.word_wrap = True

    p = tf_l.paragraphs[0]
    p.text = "Context & Challenges in Intermediate Code:"
    p.font.name = "Calibri"
    p.font.size = Pt(14)
    p.font.bold = True
    p.font.color.rgb = COLOR_BLACK
    p.space_after = Pt(10)

    points_s2 = [
        ("Redundant Computations: ", "Intermediate Three-Address Code (TAC) generated by compiler frontends repeatedly recomputes identical values, wasting CPU cycles and execution time."),
        ("Register & Memory Pressure: ", "Allocating separate temporary registers for identical expressions clutters register allocation and increases memory traffic."),
        ("Dynamic Reassignment: ", "Variables modified within a basic block must be tracked strictly so that stale values are not incorrectly reused."),
        ("Optimization Scope: ", "A Directed Acyclic Graph (DAG) exposes common subexpressions, commutative equivalences, and algebraic simplifications across basic blocks.")
    ]
    for b_title, b_desc in points_s2:
        pb = tf_l.add_paragraph()
        pb.text = "• "
        pb.font.size = Pt(11.5)
        r1 = pb.add_run()
        r1.text = b_title
        r1.font.bold = True
        r1.font.color.rgb = COLOR_BLACK
        r2 = pb.add_run()
        r2.text = b_desc
        r2.font.color.rgb = COLOR_DARK_GRAY
        pb.space_after = Pt(8)

    # Right Box: Canonical Example
    tb_right = s2.shapes.add_textbox(Inches(7.2), Inches(1.6), Inches(5.333), Inches(5.2))
    tf_r = tb_right.text_frame
    tf_r.word_wrap = True

    pr = tf_r.paragraphs[0]
    pr.text = "Optimization Goal (Before vs After):"
    pr.font.name = "Calibri"
    pr.font.size = Pt(14)
    pr.font.bold = True
    pr.font.color.rgb = COLOR_BLACK
    pr.space_after = Pt(10)

    table_s2 = s2.shapes.add_table(7, 3, Inches(7.2), Inches(2.2), Inches(5.333), Inches(4.5)).table
    table_s2.columns[0].width = Inches(1.7)
    table_s2.columns[1].width = Inches(1.7)
    table_s2.columns[2].width = Inches(1.933)

    data_s2 = [
        ("Unoptimized (Before)", "Optimized (After)", "Action Taken"),
        ("t1 = a * b", "t1 = a * b", "Initial Computation"),
        ("t2 = a * b", "t2 = t1", "CSE: Reuses Node t1"),
        ("t3 = c + d", "t3 = c + d", "Initial Computation"),
        ("t4 = t3 - t1", "t4 = t3 - t1", "Computation"),
        ("t5 = c + d", "t5 = t3", "CSE: Reuses Node t3"),
        ("t6 = t5 - t2", "t6 = t4", "Transitive Elimination")
    ]
    for row_idx, row in enumerate(data_s2):
        for col_idx, val in enumerate(row):
            cell = table_s2.cell(row_idx, col_idx)
            cell.text = val
            p_cell = cell.text_frame.paragraphs[0]
            p_cell.font.name = "Consolas" if (row_idx > 0 and col_idx < 2) else "Calibri"
            p_cell.font.size = Pt(9.5)
            if row_idx == 0:
                p_cell.font.bold = True
                cell.fill.solid()
                cell.fill.fore_color.rgb = COLOR_LIGHT_GRAY

    # =========================================================================
    # SLIDE 3: MILESTONES & REVIEW BREAKDOWN
    # =========================================================================
    s3 = prs.slides.add_slide(blank_layout)
    add_header(s3, "2. Project Review Milestones (75% Milestone Breakdown)")

    tb_m = s3.shapes.add_textbox(Inches(0.8), Inches(1.6), Inches(11.733), Inches(0.8))
    pm = tb_m.text_frame.paragraphs[0]
    pm.text = "Phased engineering roadmap from conceptual formulation to full basic block optimization:"
    pm.font.name = "Calibri"
    pm.font.size = Pt(12)
    pm.font.color.rgb = COLOR_DARK_GRAY

    m_table = s3.shapes.add_table(4, 4, Inches(0.8), Inches(2.4), Inches(11.733), Inches(4.3)).table
    m_table.columns[0].width = Inches(1.8)
    m_table.columns[1].width = Inches(1.2)
    m_table.columns[2].width = Inches(1.6)
    m_table.columns[3].width = Inches(7.133)

    m_headers = ["Milestone", "Target", "Status", "Deliverables & Implementation Scope"]
    m_rows = [
        ("Review 1", "25%", "Completed", "Problem definition, initial DAGNode struct, proof-of-concept DAG construction on toy 4-line hardcoded expression."),
        ("Review 2 (Current)", "75%", "COMPLETED", "Full TAC file parser & tokenizer, modular C++ architecture (include/ & src/), O(1) Value Numbering, Commutative CSE, Dragon Book Node Kill, Constant Folding, Algebraic Identities, Code Generator, Graphviz Visualizer, 5 Benchmark Suites."),
        ("Review 3 (Final)", "100%", "Future Scope", "Multi-block Control Flow Graph (CFG) construction, Live Variable Analysis, Global Dead Code Elimination, Target Assembly Generation (MIPS / x86).")
    ]
    for col_idx, h in enumerate(m_headers):
        cell = m_table.cell(0, col_idx)
        cell.text = h
        cp = cell.text_frame.paragraphs[0]
        cp.font.name = "Calibri"
        cp.font.size = Pt(10)
        cp.font.bold = True
        cell.fill.solid()
        cell.fill.fore_color.rgb = COLOR_LIGHT_GRAY

    for row_idx, r_data in enumerate(m_rows, start=1):
        for col_idx, text in enumerate(r_data):
            cell = m_table.cell(row_idx, col_idx)
            cell.text = text
            cp = cell.text_frame.paragraphs[0]
            cp.font.name = "Calibri"
            cp.font.size = Pt(10)
            if col_idx == 0:
                cp.font.bold = True
            if col_idx == 2 and "COMPLETED" in text:
                cp.font.bold = True

    # =========================================================================
    # SLIDE 4: SYSTEM ARCHITECTURE & MODULAR DESIGN
    # =========================================================================
    s4 = prs.slides.add_slide(blank_layout)
    add_header(s4, "3. System Architecture & Modular Codebase Layout")

    tb_s4 = s4.shapes.add_textbox(Inches(0.8), Inches(1.5), Inches(5.6), Inches(5.4))
    tf_s4 = tb_s4.text_frame
    tf_s4.word_wrap = True

    ps4 = tf_s4.paragraphs[0]
    ps4.text = "Pipeline Architecture Flow:"
    ps4.font.name = "Calibri"
    ps4.font.size = Pt(13)
    ps4.font.bold = True
    ps4.space_after = Pt(8)

    pipe_steps = [
        ("1. Input TAC File (.tac): ", "Raw Three-Address Code with comments, binary, unary, and constant instructions."),
        ("2. Tokenizer & Parser (TACParser): ", "Strips whitespace and comments (#, //), maps tokens into strongly-typed Instruction objects."),
        ("3. DAG Optimization Engine: ", "Constructs DAG, performs O(1) hash signature lookup, commutative normalization, variable detachment, constant evaluation, and algebraic reductions."),
        ("4. Code Generator: ", "Traverses and replays DAG nodes in topological order to formulate shortened, valid TAC."),
        ("5. Outputs: ", "Generates optimized TAC file and Graphviz (.dot) graph visualization.")
    ]
    for st, sd in pipe_steps:
        p_step = tf_s4.add_paragraph()
        p_step.text = "• "
        r1 = p_step.add_run()
        r1.text = st
        r1.font.bold = True
        r1.font.color.rgb = COLOR_BLACK
        r2 = p_step.add_run()
        r2.text = sd
        r2.font.color.rgb = COLOR_DARK_GRAY
        p_step.space_after = Pt(6)

    # Right Table: Modular Files
    t_mod = s4.shapes.add_table(8, 2, Inches(6.7), Inches(1.6), Inches(5.833), Inches(5.2)).table
    t_mod.columns[0].width = Inches(2.2)
    t_mod.columns[1].width = Inches(3.633)

    t_mod.cell(0, 0).text = "Module / File"
    t_mod.cell(0, 1).text = "Responsibility"
    t_mod.cell(0, 0).fill.solid()
    t_mod.cell(0, 0).fill.fore_color.rgb = COLOR_LIGHT_GRAY
    t_mod.cell(0, 1).fill.solid()
    t_mod.cell(0, 1).fill.fore_color.rgb = COLOR_LIGHT_GRAY
    t_mod.cell(0, 0).text_frame.paragraphs[0].font.bold = True
    t_mod.cell(0, 1).text_frame.paragraphs[0].font.bold = True

    mod_list = [
        ("include/Instruction.h", "InstructionType enum and TAC data structures."),
        ("include/DAGNode.h", "DAGNode structure, step records, statistics."),
        ("src/TACParser.cpp", "Dynamic file reader and token parser."),
        ("src/DAGOptimizer.cpp", "Value numbering, CSE, constant folding, code generation."),
        ("src/Visualizer.cpp", "Graphviz DOT exporter & console formatters."),
        ("src/BenchmarkRunner.cpp", "Automated test suite execution logic."),
        ("src/main.cpp", "CLI argument parser and interactive UI menu.")
    ]
    for idx, (mf, md) in enumerate(mod_list, start=1):
        c0 = t_mod.cell(idx, 0)
        c1 = t_mod.cell(idx, 1)
        c0.text = mf
        c1.text = md
        c0.text_frame.paragraphs[0].font.name = "Consolas"
        c0.text_frame.paragraphs[0].font.size = Pt(8.5)
        c1.text_frame.paragraphs[0].font.name = "Calibri"
        c1.text_frame.paragraphs[0].font.size = Pt(8.5)

    # =========================================================================
    # SLIDE 5: OPTIMIZATION TECHNIQUES (CSE & COMMUTATIVITY)
    # =========================================================================
    s5 = prs.slides.add_slide(blank_layout)
    add_header(s5, "4. Optimization Techniques: CSE & Commutative Matching")

    tb_s5 = s5.shapes.add_textbox(Inches(0.8), Inches(1.6), Inches(11.733), Inches(5.3))
    tf_s5 = tb_s5.text_frame
    tf_s5.word_wrap = True

    p = tf_s5.paragraphs[0]
    p.text = "A. Common Subexpression Elimination (CSE) via O(1) Value Numbering"
    p.font.name = "Calibri"
    p.font.size = Pt(13)
    p.font.bold = True
    p.font.color.rgb = COLOR_BLACK
    p.space_after = Pt(4)

    p_cse = tf_s5.add_paragraph()
    p_cse.text = "• Rather than looping through previous instructions in O(N^2) time, the optimizer computes a signature for each operation: sig = op(leftId, rightId).\n• A hash map lookup (valueNumbering[sig]) verifies uniqueness in O(1) average time.\n• If found, the existing node's primary variable is reused, eliminating duplicate computation."
    p_cse.font.size = Pt(10.5)
    p_cse.font.color.rgb = COLOR_DARK_GRAY
    p_cse.space_after = Pt(12)

    p_com_t = tf_s5.add_paragraph()
    p_com_t.text = "B. Commutative Operator Normalization"
    p_com_t.font.name = "Calibri"
    p_com_t.font.size = Pt(13)
    p_com_t.font.bold = True
    p_com_t.font.color.rgb = COLOR_BLACK
    p_com_t.space_after = Pt(4)

    p_com = tf_s5.add_paragraph()
    p_com.text = "• In mathematics, operations such as a + b and b + a are identical, but naive compiler passes treat them as separate.\n• Our optimizer identifies commutative operators: +, *, &, |, ^, ==, !=\n• Canonical signature: If isCommutative(op) and leftId > rightId, the operand IDs are sorted min(leftId, rightId), max(leftId, rightId).\n• Guarantees that expressions with swapped operands map to the exact same DAG node."
    p_com.font.size = Pt(10.5)
    p_com.font.color.rgb = COLOR_DARK_GRAY
    p_com.space_after = Pt(10)

    # Commutative Table Example
    t_com = s5.shapes.add_table(3, 3, Inches(0.8), Inches(5.1), Inches(11.733), Inches(1.8)).table
    t_com.columns[0].width = Inches(3.5)
    t_com.columns[1].width = Inches(3.5)
    t_com.columns[2].width = Inches(4.733)
    
    t_com.cell(0, 0).text = "Input TAC"
    t_com.cell(0, 1).text = "Generated Signature"
    t_com.cell(0, 2).text = "Optimization Action"
    for ci in range(3):
        t_com.cell(0, ci).fill.solid()
        t_com.cell(0, ci).fill.fore_color.rgb = COLOR_LIGHT_GRAY
        t_com.cell(0, ci).text_frame.paragraphs[0].font.bold = True

    t_com.cell(1, 0).text = "t1 = a + b"
    t_com.cell(1, 1).text = "+(0, 1)"
    t_com.cell(1, 2).text = "Creates Node 2 (Primary: t1)"

    t_com.cell(2, 0).text = "t2 = b + a"
    t_com.cell(2, 1).text = "+(0, 1) [Sorted]"
    t_com.cell(2, 2).text = "Reuses Node 2 -> Emits 't2 = t1' (Redundant Cut)"

    for r in range(1, 3):
        for c in range(3):
            cell = t_com.cell(r, c)
            cell.text_frame.paragraphs[0].font.name = "Consolas" if c < 2 else "Calibri"
            cell.text_frame.paragraphs[0].font.size = Pt(9.5)

    # =========================================================================
    # SLIDE 6: OPTIMIZATION TECHNIQUES (NODE KILL & SIMPLIFICATIONS)
    # =========================================================================
    s6 = prs.slides.add_slide(blank_layout)
    add_header(s6, "5. Optimization Techniques: Node Kill & Constant Folding")

    tb_s6 = s6.shapes.add_textbox(Inches(0.8), Inches(1.5), Inches(5.6), Inches(5.4))
    tf_s6 = tb_s6.text_frame
    tf_s6.word_wrap = True

    p = tf_s6.paragraphs[0]
    p.text = "Dragon Book Variable Reassignment & Kill:"
    p.font.name = "Calibri"
    p.font.size = Pt(13)
    p.font.bold = True
    p.space_after = Pt(6)

    kill_points = [
        "• Challenge: If variable 'a' is reassigned inside a basic block (e.g. a = 10), subsequent uses of 'a' must not reference the old value.",
        "• Detach Mechanism: detachVariable(a) removes 'a' from the previous node's active labels list and clears varToNode[a].",
        "• Rebind: attachVariable(a, newNodeId) attaches 'a' to the newly computed node.",
        "• Result: Prevents semantic aliasing bugs while preserving valid expressions using previous variables."
    ]
    for kp in kill_points:
        pk = tf_s6.add_paragraph()
        pk.text = kp
        pk.font.size = Pt(10)
        pk.font.color.rgb = COLOR_DARK_GRAY
        pk.space_after = Pt(4)

    # Right Box: Constant Folding & Identities
    tb_s6_r = s6.shapes.add_textbox(Inches(6.8), Inches(1.5), Inches(5.733), Inches(5.4))
    tf_s6_r = tb_s6_r.text_frame
    tf_s6_r.word_wrap = True

    pr = tf_s6_r.paragraphs[0]
    pr.text = "Constant Folding & Algebraic Identities:"
    pr.font.name = "Calibri"
    pr.font.size = Pt(13)
    pr.font.bold = True
    pr.space_after = Pt(6)

    t_simp = s6.shapes.add_table(7, 3, Inches(6.8), Inches(2.2), Inches(5.733), Inches(4.5)).table
    t_simp.columns[0].width = Inches(1.8)
    t_simp.columns[1].width = Inches(1.4)
    t_simp.columns[2].width = Inches(2.533)

    t_simp.cell(0, 0).text = "Identity / Rule"
    t_simp.cell(0, 1).text = "Input TAC"
    t_simp.cell(0, 2).text = "Simplified Output"
    for ci in range(3):
        t_simp.cell(0, ci).fill.solid()
        t_simp.cell(0, ci).fill.fore_color.rgb = COLOR_LIGHT_GRAY
        t_simp.cell(0, ci).text_frame.paragraphs[0].font.bold = True

    simp_rows = [
        ("Constant Arithmetic", "t1 = 12 + 8", "t1 = 20 (Folded)"),
        ("Constant Propagation", "t2 = t1 * 2", "t2 = 40 (Folded)"),
        ("Identity: x + 0 = x", "t3 = b + 0", "t3 = b (Identity)"),
        ("Identity: x * 1 = x", "t4 = a * 1", "t4 = a (Identity)"),
        ("Identity: x * 0 = 0", "t5 = c * 0", "t5 = 0 (Annihilator)"),
        ("Identity: x - x = 0", "t6 = d - d", "t6 = 0 (Zero Rule)")
    ]
    for r_idx, (r0, r1, r2) in enumerate(simp_rows, start=1):
        t_simp.cell(r_idx, 0).text = r0
        t_simp.cell(r_idx, 1).text = r1
        t_simp.cell(r_idx, 2).text = r2
        for ci in range(3):
            cell = t_simp.cell(r_idx, ci)
            cell.text_frame.paragraphs[0].font.name = "Consolas" if ci > 0 else "Calibri"
            cell.text_frame.paragraphs[0].font.size = Pt(9)

    # =========================================================================
    # SLIDE 7: CODE GENERATION & DAG VISUALIZATION
    # =========================================================================
    s7 = prs.slides.add_slide(blank_layout)
    add_header(s7, "6. Code Generation & Visual DAG Export")

    tb_s7 = s7.shapes.add_textbox(Inches(0.8), Inches(1.6), Inches(5.6), Inches(5.3))
    tf_s7 = tb_s7.text_frame
    tf_s7.word_wrap = True

    p = tf_s7.paragraphs[0]
    p.text = "Chronological Replay Code Generation:"
    p.font.name = "Calibri"
    p.font.size = Pt(13)
    p.font.bold = True
    p.space_after = Pt(8)

    cg_points = [
        ("Execution Order Replay: ", "Walks the sequential processed steps rather than an arbitrary traversal, ensuring variable lifetimes remain strictly correct."),
        ("Primary Variable Binding: ", "When emitting an operation, it looks up the currently valid active representative variable for child nodes."),
        ("Clean Alias Emission: ", "For expressions identified as common subexpressions, emits a lightweight copy assignment (e.g. t2 = t1), guaranteeing live-out variables remain defined."),
        ("Dead Temporary Elimination: ", "Optional aggressive mode eliminates intermediate temporary copies when variables are not used outside the basic block.")
    ]
    for cgt, cgd in cg_points:
        pcg = tf_s7.add_paragraph()
        pcg.text = "• "
        r1 = pcg.add_run()
        r1.text = cgt
        r1.font.bold = True
        r1.font.color.rgb = COLOR_BLACK
        r2 = pcg.add_run()
        r2.text = cgd
        r2.font.color.rgb = COLOR_DARK_GRAY
        pcg.space_after = Pt(6)

    # Right Box: Graphviz Export
    tb_s7_r = s7.shapes.add_textbox(Inches(6.8), Inches(1.6), Inches(5.733), Inches(5.3))
    tf_s7_r = tb_s7_r.text_frame
    tf_s7_r.word_wrap = True

    pr = tf_s7_r.paragraphs[0]
    pr.text = "Automated Graphviz DOT Exporter:"
    pr.font.name = "Calibri"
    pr.font.size = Pt(13)
    pr.font.bold = True
    pr.space_after = Pt(8)

    gv_points = [
        ("Direct Graph Serialization: ", "The system automatically exports finalized DAG nodes and directed edges to standard .dot format (dag_output.dot)."),
        ("Node Categorization: ", "Leaf variables, constant nodes, and internal operator nodes are tagged with distinct labels and active variables."),
        ("Visual Proof of Merging: ", "Multiple variables attached to a single computation node visually verify Common Subexpression Elimination for review panels."),
        ("Instant Rendering: ", "Compatible with Graphviz CLI (dot -Tpng) and browser viewers (Graphviz Online, Edotor).")
    ]
    for gvt, gvd in gv_points:
        pgv = tf_s7_r.add_paragraph()
        pgv.text = "• "
        r1 = pgv.add_run()
        r1.text = gvt
        r1.font.bold = True
        r1.font.color.rgb = COLOR_BLACK
        r2 = pgv.add_run()
        r2.text = gvd
        r2.font.color.rgb = COLOR_DARK_GRAY
        pgv.space_after = Pt(6)

    # =========================================================================
    # SLIDE 8: SYSTEM VERIFICATION (TESTCASE 1 LIVE AUDIT)
    # =========================================================================
    s8 = prs.slides.add_slide(blank_layout)
    add_header(s8, "7. System Verification: Execution Audit on Benchmark 1")

    # Left: Terminal Output Box
    tb_term = s8.shapes.add_textbox(Inches(0.8), Inches(1.5), Inches(6.0), Inches(5.4))
    tf_term = tb_term.text_frame
    tf_term.word_wrap = True

    p = tf_term.paragraphs[0]
    p.text = "Verified Execution Output (Benchmark 1 - README):"
    p.font.name = "Calibri"
    p.font.size = Pt(12)
    p.font.bold = True
    p.space_after = Pt(6)

    term_code = (
        "[INFO] Processing TAC file: samples/sample1_readme.tac\n\n"
        "--- Optimization Pipeline Audit Log ---\n"
        "  1. CSE: 't2 = a * b' maps to existing Node 2 [Rep: t1]\n"
        "  2. CSE: 't5 = c + d' maps to existing Node 5 [Rep: t3]\n"
        "  3. CSE: 't6 = t5 - t2' maps to existing Node 6 [Rep: t4]\n\n"
        "--- Constructed DAG Nodes ---\n"
        "Node 0: 'a'  | Labels: [a]\n"
        "Node 1: 'b'  | Labels: [b]\n"
        "Node 2: '*'  | Left: 0, Right: 1 | Labels: [t1, t2]\n"
        "Node 3: 'c'  | Labels: [c]\n"
        "Node 4: 'd'  | Labels: [d]\n"
        "Node 5: '+'  | Left: 3, Right: 4 | Labels: [t3, t5]\n"
        "Node 6: '-'  | Left: 5, Right: 2 | Labels: [t4, t6]\n\n"
        "[SUCCESS] Exported DAG to Graphviz: dag_output.dot"
    )
    p_code = tf_term.add_paragraph()
    p_code.text = term_code
    p_code.font.name = "Consolas"
    p_code.font.size = Pt(8.5)
    p_code.font.color.rgb = COLOR_BLACK

    # Right: Transformation Table & Metrics
    t_v = s8.shapes.add_table(7, 2, Inches(7.1), Inches(1.6), Inches(5.433), Inches(3.6)).table
    t_v.columns[0].width = Inches(2.7)
    t_v.columns[1].width = Inches(2.733)

    t_v.cell(0, 0).text = "Input TAC (Before)"
    t_v.cell(0, 1).text = "Optimized Output (After)"
    t_v.cell(0, 0).fill.solid()
    t_v.cell(0, 0).fill.fore_color.rgb = COLOR_LIGHT_GRAY
    t_v.cell(0, 1).fill.solid()
    t_v.cell(0, 1).fill.fore_color.rgb = COLOR_LIGHT_GRAY
    t_v.cell(0, 0).text_frame.paragraphs[0].font.bold = True
    t_v.cell(0, 1).text_frame.paragraphs[0].font.bold = True

    v_rows = [
        ("t1 = a * b", "t1 = a * b"),
        ("t2 = a * b", "t2 = t1  (CSE Reused)"),
        ("t3 = c + d", "t3 = c + d"),
        ("t4 = t3 - t1", "t4 = t3 - t1"),
        ("t5 = c + d", "t5 = t3  (CSE Reused)"),
        ("t6 = t5 - t2", "t6 = t4  (Transitive Cut)")
    ]
    for idx, (vb, va) in enumerate(v_rows, start=1):
        c0 = t_v.cell(idx, 0)
        c1 = t_v.cell(idx, 1)
        c0.text = vb
        c1.text = va
        c0.text_frame.paragraphs[0].font.name = "Consolas"
        c0.text_frame.paragraphs[0].font.size = Pt(9)
        c1.text_frame.paragraphs[0].font.name = "Consolas"
        c1.text_frame.paragraphs[0].font.size = Pt(9)

    # Metrics Summary Box on Right
    tb_m = s8.shapes.add_textbox(Inches(7.1), Inches(5.4), Inches(5.433), Inches(1.6))
    tf_mb = tb_m.text_frame
    tf_mb.word_wrap = True

    pm0 = tf_mb.paragraphs[0]
    pm0.text = "EMPIRICAL METRICS SUMMARY:"
    pm0.font.name = "Calibri"
    pm0.font.size = Pt(11)
    pm0.font.bold = True
    pm0.space_after = Pt(4)

    metrics_text = [
        "• Total Original Instructions: 6",
        "• Redundant Computations Eliminated: 3 (50.0% Reduction)",
        "• Value Numbering Lookups: 100% O(1) Constant Time"
    ]
    for mt in metrics_text:
        pm = tf_mb.add_paragraph()
        pm.text = mt
        pm.font.name = "Calibri"
        pm.font.size = Pt(10)
        pm.font.color.rgb = COLOR_DARK_GRAY

    # =========================================================================
    # SLIDE 9: EMPIRICAL BENCHMARK EVALUATION
    # =========================================================================
    s9 = prs.slides.add_slide(blank_layout)
    add_header(s9, "8. Empirical Benchmark Evaluation (5 Test Cases)")

    tb_s9 = s9.shapes.add_textbox(Inches(0.8), Inches(1.5), Inches(11.733), Inches(0.6))
    p = tb_s9.text_frame.paragraphs[0]
    p.text = "Comprehensive performance and correctness evaluation across distinct intermediate code patterns:"
    p.font.name = "Calibri"
    p.font.size = Pt(11.5)
    p.font.color.rgb = COLOR_DARK_GRAY

    t_bm = s9.shapes.add_table(6, 6, Inches(0.8), Inches(2.2), Inches(11.733), Inches(4.5)).table
    t_bm.columns[0].width = Inches(2.2)
    t_bm.columns[1].width = Inches(2.6)
    t_bm.columns[2].width = Inches(1.1)
    t_bm.columns[3].width = Inches(1.2)
    t_bm.columns[4].width = Inches(1.3)
    t_bm.columns[5].width = Inches(3.333)

    bm_headers = ["Benchmark Suite", "Input File", "Initial", "Optimized", "Redundant Cut", "Key Optimization Demonstrated"]
    for ci, h in enumerate(bm_headers):
        cell = t_bm.cell(0, ci)
        cell.text = h
        cell.fill.solid()
        cell.fill.fore_color.rgb = COLOR_LIGHT_GRAY
        p_h = cell.text_frame.paragraphs[0]
        p_h.font.name = "Calibri"
        p_h.font.size = Pt(9.5)
        p_h.font.bold = True

    bm_data = [
        ("Test 1: Readme CSE", "sample1_readme.tac", "6", "6", "3 (50.0%)", "Standard CSE and transitive expression reuse."),
        ("Test 2: Commutativity", "sample2_commutative.tac", "6", "6", "3 (50.0%)", "Commutative equivalence on '+' and '*' operators."),
        ("Test 3: Reassignment", "sample3_reassignment.tac", "6", "6", "1 (16.7%)", "Dragon Book node kill & label detach validation."),
        ("Test 4: Constant/Identities", "sample4_constant_folding.tac", "7", "7", "7 (100.0%)", "3 constant foldings + 4 algebraic identities."),
        ("Test 5: Matrix Indexing", "sample5_matrix_offset.tac", "13", "13", "6 (46.2%)", "Repeated 2D array offset address calculations.")
    ]
    for ri, row in enumerate(bm_data, start=1):
        for ci, val in enumerate(row):
            cell = t_bm.cell(ri, ci)
            cell.text = val
            p_c = cell.text_frame.paragraphs[0]
            p_c.font.name = "Consolas" if ci == 1 else "Calibri"
            p_c.font.size = Pt(9.5)
            if ci == 0 or ci == 4:
                p_c.font.bold = True

    # =========================================================================
    # SLIDE 10: CONCLUSION & FUTURE WORK (REVIEW 3)
    # =========================================================================
    s10 = prs.slides.add_slide(blank_layout)
    add_header(s10, "9. Conclusion & Plan for Review 3 (100% Scope)")

    tb_s10_l = s10.shapes.add_textbox(Inches(0.8), Inches(1.6), Inches(5.6), Inches(5.2))
    tf_s10_l = tb_s10_l.text_frame
    tf_s10_l.word_wrap = True

    pl = tf_s10_l.paragraphs[0]
    pl.text = "Summary of Review 2 Deliverables (75%):"
    pl.font.name = "Calibri"
    pl.font.size = Pt(13)
    pl.font.bold = True
    pl.space_after = Pt(8)

    r2_summary = [
        ("Complete Basic Block Engine: ", "Transitioned successfully from a toy hardcoded concept to a production-grade, modular C++ optimizer."),
        ("Robust Optimization Suite: ", "Value Numbering, Commutative Matching, Reassignment Node Kill, Constant Folding, and Algebraic Reductions fully implemented and validated."),
        ("Verified Code Generation: ", "Reconstructs valid, optimized TAC files and exports Graphviz .dot visual DAG diagrams."),
        ("Zero Warnings Build: ", "Compiles cleanly under -Wall -Wextra with automated GNU Makefile, PowerShell, and Batch test scripts.")
    ]
    for st, sd in r2_summary:
        p_sum = tf_s10_l.add_paragraph()
        p_sum.text = "• "
        r1 = p_sum.add_run()
        r1.text = st
        r1.font.bold = True
        r1.font.color.rgb = COLOR_BLACK
        r2 = p_sum.add_run()
        r2.text = sd
        r2.font.color.rgb = COLOR_DARK_GRAY
        p_sum.space_after = Pt(6)

    # Right Box: Review 3 Roadmap
    tb_s10_r = s10.shapes.add_textbox(Inches(6.8), Inches(1.6), Inches(5.733), Inches(5.2))
    tf_s10_r = tb_s10_r.text_frame
    tf_s10_r.word_wrap = True

    pr = tf_s10_r.paragraphs[0]
    pr.text = "Remaining Scope for Review 3 (100% Final):"
    pr.font.name = "Calibri"
    pr.font.size = Pt(13)
    pr.font.bold = True
    pr.space_after = Pt(8)

    r3_points = [
        ("Control Flow Graph (CFG) Construction: ", "Partition multi-block intermediate code into leaders, basic blocks, and directed control edges for branches and jumps."),
        ("Global Dataflow Analysis: ", "Implement Reaching Definitions and Live Variable Analysis across multiple block boundaries."),
        ("Global Dead Code Elimination: ", "Eliminate dead stores and unused temporary registers globally."),
        ("Target Assembly Code Emission: ", "Translate optimized intermediate representations to target architecture instructions (MIPS or x86).")
    ]
    for r3t, r3d in r3_points:
        pr3 = tf_s10_r.add_paragraph()
        pr3.text = "• "
        r1 = pr3.add_run()
        r1.text = r3t
        r1.font.bold = True
        r1.font.color.rgb = COLOR_BLACK
        r2 = pr3.add_run()
        r2.text = r3d
        r2.font.color.rgb = COLOR_DARK_GRAY
        pr3.space_after = Pt(6)

    # Save presentation
    output_filename = "CD_LAB_ProjRev2_Presentation.pptx"
    prs.save(output_filename)
    print(f"[SUCCESS] Successfully generated presentation: {output_filename}")

if __name__ == '__main__':
    create_deck()
