//
// Created by Mhnd on 02/11/2022.
//

#ifndef FUSEBMC_INSTRUMENT_INTERVAL_APPROACH_H_I
#define FUSEBMC_INSTRUMENT_INTERVAL_APPROACH_H_I

#include <iostream>
#include <ibex.h>
#include <ibex/ibex_Interval.h>
#include <ibex/ibex_Expr.h>
#include <ibex/ibex_Ctc.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/DeclCXX.h>
#include <clang/Rewrite/Core/Rewriter.h>


class vart {
private:
    ibex::Interval interval;
    std::string var_name;
    clang::VarDecl *symbol;
    size_t index;
    bool interval_changed;

public:
    size_t getIndex() const;

public:
    vart(const string &varName, clang::VarDecl *symbol, const size_t &index);

    const ibex::Interval &getInterval() const;

    void setInterval(const ibex::Interval &interval);

    bool isIntervalChanged() const;

    void setIntervalChanged(bool intervalChanged);

    clang::VarDecl *getSymbol() const;
};

class Contractor {
    ibex::Ctc *outer;
    ibex::Ctc *inner;
    unsigned int location;

public:
    Contractor(ibex::Ctc *c, unsigned int loc) {
        outer = c;
        location = loc;
        //inner = get_complement_contractor(c);
    }

    Contractor() = default;

    void set_outer(ibex::Ctc *outer) {
        Contractor::outer = outer;
    }

    void set_inner(ibex::Ctc *inner) {
        Contractor::inner = inner;
    }

    void set_location(unsigned int location) {
        Contractor::location = location;
    }

    ibex::Ctc *get_outer() const {
        return outer;
    }

    ibex::Ctc *get_inner() const {
        return inner;
    }

    unsigned int get_location() const {
        return location;
    }

private:
    /*ibex::Ctc *get_complement_contractor(ibex::Ctc *c) {
        if (auto ctc_compo = dynamic_cast<ibex::CtcCompo *>(c)) {
            ibex::Array<ibex::Ctc> list_of_contractors;
            for (auto &it: ctc_compo->list)
                list_of_contractors.add(*get_complement_contractor(&it));
            return new ibex::CtcUnion(list_of_contractors);
        } else if (auto ctc_union = dynamic_cast<ibex::CtcUnion *>(c)) {
            ibex::Array<ibex::Ctc> list_of_contractors;
            for (auto &it: ctc_union->list)
                list_of_contractors.add(*get_complement_contractor(&it));
            return new ibex::CtcCompo(list_of_contractors);
        } else if (auto fwdbwd = dynamic_cast<ibex::CtcFwdBwd *>(c)) {
            ibex::NumConstraint *ctr;
            switch (fwdbwd->ctr.op) {
                case ibex::GEQ:
                    ctr = new ibex::NumConstraint(fwdbwd->ctr.f, ibex::LT);
                    return new ibex::CtcFwdBwd(*ctr);
                case ibex::GT:
                    ctr = new ibex::NumConstraint(fwdbwd->ctr.f, ibex::LEQ);
                    return new ibex::CtcFwdBwd(*ctr);
                case ibex::LEQ:
                    ctr = new ibex::NumConstraint(fwdbwd->ctr.f, ibex::GT);
                    return new ibex::CtcFwdBwd(*ctr);
                case ibex::LT:
                    ctr = new ibex::NumConstraint(fwdbwd->ctr.f, ibex::GEQ);
                    return new ibex::CtcFwdBwd(*ctr);
                case ibex::EQ:
                    ctr = new ibex::NumConstraint(fwdbwd->ctr.f, ibex::GT);
                    auto ctr2 = new ibex::NumConstraint(fwdbwd->ctr.f, ibex::LT);
                    auto *side1 = new ibex::CtcFwdBwd(*ctr);
                    auto *side2 = new ibex::CtcFwdBwd(*ctr2);
                    return new ibex::CtcUnion(*side1, *side2);
            }
        }
        //debug log error
        return nullptr;
    } */
};

class Contractors {
    std::list<Contractor *> contractors;

public:
    Contractor *get_contractors_up_to_loc(unsigned int loc) {
        Contractor *c = new Contractor();

        auto size = contractors.size();
        ibex::Array<ibex::Ctc> outer[size];
        ibex::Array<ibex::Ctc> inner[size];
        for (auto const &ctc: contractors)
            if (ctc->get_location() < loc) {
                outer->add(*(ctc->get_outer()));
                inner->add(*(ctc->get_inner()));
            }

        c->set_outer(new ibex::CtcCompo(*outer));
        c->set_inner(new ibex::CtcUnion(*inner));
        c->set_location(0);
        return c;
    }

    Contractor *get_contractors() {
        Contractor *c = new Contractor();

        auto size = contractors.size();
        ibex::Array<ibex::Ctc> outer[size];
        //ibex::Array<ibex::Ctc> inner[size];
        for (auto const &ctc: contractors) {
            outer->add(*(ctc->get_outer()));
            //inner->add(*(ctc->get_inner()));
        }
        if(outer->size() == 0)
            c->set_outer(nullptr);
        else
            c->set_outer(new ibex::CtcCompo(*outer));
        // c->set_inner(new ibex::CtcUnion(*inner));
        c->set_location(0);
        return c;
    }

    /*void dump() {
        std::ostringstream oss;

        for (auto const &c: contractors) {
            oss << "outer :" << to_oss(c->get_outer()).str() << "\n";
           // oss << "inner :" << to_oss(c->get_inner()).str() << "\n";
           // oss << "location :" << c->get_location() << "\n";
        }
        std::cout << oss.str() << std::endl;
    }

    /*std::ostringstream list_to_oss(ibex::Array<ibex::Ctc> *list, bool is_compo) {
        std::ostringstream oss;
        auto it = list->begin();
        oss << "( " << to_oss(&*it).str();
        it++;
        while (it != list->end()) {
            oss << (is_compo ? " && " : " || ") << to_oss(&*it).str();
            it++;
        }
        oss << " )";
        return oss;
    }

   /*std::ostringstream to_oss(ibex::Ctc *c) {
        std::ostringstream oss;
        try{
            //clang::isa<ibex::CtcCompo>(c);
            auto ctc_compo = static_cast<ibex::CtcUnion *>(c);
            oss = list_to_oss(&ctc_compo->list, true);
        }
        catch(exception e)
        {
            oss << "NO" <<std::endl;
        }

        /*else if (auto ctc_union = dynamic_cast<ibex::CtcUnion *>(c))
            oss = list_to_oss(&ctc_union->list, false);
        else if (auto fwdbwd = dynamic_cast<ibex::CtcFwdBwd *>(c))
            oss << fwdbwd->ctr;

        return oss;
    }*/

    void add_contractor(ibex::Ctc *ctc, unsigned int loc) {
        if (ctc != nullptr) {
            Contractor *c = new Contractor(ctc, loc);
            contractors.insert(contractors.end(), c);
        }
    }

    bool is_empty() {
        return contractors.empty();
    }
};

/**
 * This class is for mapping the variables with their names and intervals.
 * It includes functionalities such as search for a variable by name and add
 * a new variable. Also update a variable interval by changing the upper/lower
 * limit.
 */
class CspMap {
public:
    static constexpr int MAX_VAR = 100;
    static constexpr int NOT_FOUND = -1;

    std::map<std::string, vart *> var_map;

    CspMap() {
    }

    size_t add_var(const std::string &name, clang::VarDecl *symbol) {
        auto find = var_map.find(name);
        if (find == var_map.end()) {
            //TODO: set initial intervals based on type and width.
            interval x(NEG_INFINITY, POS_INFINITY);
            vart *var = new vart(name, symbol, n);
            var->setInterval(x);
            var_map[name] = var;
            n++;
            return n - 1;
        }
        return find->second->getIndex();
    }

    void update_lb_interval(double lb, const std::string &name) {
        auto find = var_map.find(name);
        if (find == var_map.end())
            return;
        ibex::Interval X(lb, find->second->getInterval().ub());
        find->second->setInterval(X);
    }

    void update_ub_interval(double ub, const std::string &name) {
        auto find = var_map.find(name);
        if (find == var_map.end())
            return;
        ibex::Interval X(find->second->getInterval().lb(), ub);
        find->second->setInterval(X);
    }

    int find(const std::string &name) {
        auto find = var_map.find(name);
        if (find == var_map.end())
            return NOT_FOUND;
        return find->second->getIndex();
    }

    size_t size() const {
        return n;
    }

    ibex::IntervalVector create_interval_vector() {
        ibex::IntervalVector X(var_map.size());
        for (auto const &var: var_map)
            X[var.second->getIndex()] = var.second->getInterval();
        return X;
    }

    void update_intervals(ibex::IntervalVector vector) {
        //check if interval box is empty set or if the interval is degenerated
        // in the case of a single interval
        if (vector.is_empty())
            is_empty_vector = true;

        if (vector.size() == 1 && vector[0].is_degenerated())
            return;

        for (auto &var: var_map) {
            if (var.second->getInterval() != vector[var.second->getIndex()]) {
                var.second->setInterval(vector[var.second->getIndex()]);
                var.second->setIntervalChanged(true);
            }
        }
    }

private:
    size_t n = 0;
    bool is_empty_vector = false;

public:
    bool is_empty_set() const {
        return is_empty_vector;
    }
};

class goto_contractort {
public:
    /**
     * This constructor will run the goto-contractor procedure.
     * it will go through 4 steps.
     * First is parsing the properties.
     * Second, parsing the intervals.
     * Third, applying the contractor.
     * Fourth, inserting assumes in the program to reflect the contracted intervals.
     * @param _goto_functions
     */
    goto_contractort() {
        vars = new ibex::Variable(CspMap::MAX_VAR);
        /*
        initialize_main_function_loops();
        if (true) {

            std::cout << "1/4 - Parsing asserts to create CSP Constraints." << std::endl;
            //get_contractors();
            if (contractors.is_empty()) {
                std::cout << "Contractors: expression not supported, No Contractors were created." << std::endl;
                return;
            }
            //contractors.dump();
            std::cout << "2/4 - Parsing assumes to set values for variables intervals."<< std::endl;
            get_intervals();

            std::cout << "3/4 - Applying contractor."<< std::endl;
            apply_contractor();

            std::cout << "4/4 - Inserting assumes."<< std::endl;
            insert_assume();
        }*/
    }

/// \Function get_contractors is a function that will go through each asert
/// in the program and parse it from ESBMC expression to an IBEX expression
/// that will be added create two contractors with the constraints.
/// One is for the outer contractor with the constraint of the assert
/// condition, another for the inner with the complement of the constraint.
/// the function will return nothing. However the contractors be added to
/// the list of contractors.
/// \param functionst list of functions in the goto program
    void get_contractors(clang::Expr *expr);

public:
    ibex::IntervalVector domains;
    ///vars variable references to be used in Ibex formulas
    ibex::Variable *vars;
    /// map is where the variable references and intervals are stored.
    CspMap map;
    /// contractors is where all the contractors and their complement are stored.
    Contractors contractors;

    unsigned number_of_functions = 0;

    //typedef std::list<loopst> function_loopst;
    //function_loopst function_loops;

    /// \Function get_intervals is a function that will go through each asert in
    /// the program and parse it from ESBMC expression to a triplet that are the
    /// variable name and and update its interval depending on the relation it
    /// will decide if the lower or the upper limit or both. the function will
    /// return nothing. However the values of the intervals of each variable will
    /// be updated in the Map that holds the variable information.
    /// \param functionst list of functions in the goto program
    void get_intervals();

    /// \Function contractor function will apply the contractor on the parsed
    /// constraint and intervals. it will apply the inner contractor by
    /// calculating the complement of the assert and contract.
    /// \return Interval vector that represents the area that should be checked
    /// by the bmc.
    void apply_contractor();

    /**
     * @function insert_assume is the function that will use the intervals
     * produced by the contractor and compare it to the original intervals.
     * If there are any changes, it will be inserted into the program as assumes
     * in the format of assume(<variable> <operator> <value>) where <variable> is
     * the variable name, <operator> is <=,>= depending if its an upper or a lower
     * limit and <value> is the value of the interval limit. If the resulting
     * interval is empty (check via is_empty_vector flag), it will insert
     * assume(0). It will also search for the last loop in the program based
     * on location.
     * @param goto_functions goto program functions
     * @param vector result from the contractor.
     */
    void insert_assume();

    bool is_constraint_operator(clang::Expr *);

    bool is_unsupported_operator_in_constraint(clang::Expr *);

    /**
     * @function create_contractor_from_expr2t is called from get_contractors
     * where we parse the property to create constraints for our CSP.
     * This function handles logical operators && and || to create a composition
     * or union between contractors. With one exception for !=, because ibex does
     * not have such operator and it can be expressed with a union between >
     * and < for the same property. For example, x != y will be (x > y || x < y)
     * @return ibex::Ctc * contractor
     */
    ibex::Ctc *create_contractor_from_expr2t(clang::Expr *);

    /**
     * @function create_constraint_from_expr2t is called from create_contractor_from_expr2t
     * and it will parse an expression with comparison operators and create
     * constraints to be used by create_contractor_from_expr2t.
     * @return Constraint
     */
    ibex::NumConstraint *create_constraint_from_expr2t(clang::Expr *);

    /**
     * @function create_function_from_expr2t is called by create_constraint_from_expr2t
     * and it will parse expressions with arithmetic operators and create
     * functions to be used by create_constraint_from_expr2t.
     * @return Function
     */
    ibex::Function *create_function_from_expr2t(clang::Expr *);

    int create_variable_from_expr2t(clang::VarDecl *);

    void parse_error(clang::Expr *);

    void parse_intervals(clang::Expr);

    bool initialize_main_function_loops();

    clang::Expr *get_base_object(clang::Expr *expr);

    string get_list_of_vars() {
        string list;
        if (this->map.var_map.empty())
            return "  ";
        for (auto &i: this->map.var_map) {
            list += ",";
            list += i.first;
        }
        return list.erase(0, 1);
    }

    string get_intervals_list() {
        string list = "";
        //std::cout << "Result" << std::endl;
        if (map.is_empty_set())
            list += "Unreachable\n";
        else
            for (auto &i: this->map.var_map)
                list += i.first + "\n" + std::to_string(i.second->getInterval().lb()) + "\n" +
                        std::to_string(i.second->getInterval().ub()) + "\n";
        //list+= "end of Result" << std::endl;
        return list;
    }

    void min_max_bounds(double *min, double *max) {
        //*min = POS_INFINITY;
        //*max = NEG_INFINITY;

        if (map.is_empty_set())
            return;
        else
            for (auto &i: this->map.var_map) {
                if (i.second->getInterval().lb() != NEG_INFINITY && i.second->getInterval().lb() < *min)
                    *min = i.second->getInterval().lb();
                if (i.second->getInterval().ub() != POS_INFINITY && i.second->getInterval().ub() > *max)
                    *max = i.second->getInterval().ub();
            }


    }
};

#endif //FUSEBMC_INSTRUMENT_INTERVAL_APPROACH_H_I
