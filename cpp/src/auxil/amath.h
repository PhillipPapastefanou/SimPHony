//
// Created by ppapastefanou on 4/8/24.
//

#pragma once
#include <math.h>
#include <algorithm>

const double DHUGE     = std::numeric_limits<double>::max();
const long LHUGE       = std::numeric_limits<long>::max()  ;
const int IHUGE	       = std::numeric_limits<int>::max()   ;
const double D_EPSILON = std::numeric_limits<double>::min();


/* Evaluate the Normal 01 CDF.
 *
 * Licensing:
 *     This code is distributed under the GNU LGPL license.
 * Modified:
 *     - 10 February 1999
 *     - Extracted: June, 2016
 * Author:
 *     - John Burkardt
 *     - Extracted by Philipp Sommer
 * Reference:
 *     AG Adams,
 *     Algorithm 39,
 *     Areas Under the Normal Curve,
 *     Computer Journal,
 *     Volume 12, pages 197-198, 1969.
 * Input
 *  x   : the argument of the CDF.
 *  cdf : the value of the CDF.
 */
void normal_01_cdf ( double x, double cdf ) {

    const double A1 = 0.398942280444E+00 ;
    const double A2 = 0.399903438504E+00 ;
    const double A3 = 5.75885480458E+00  ;
    const double A4 = 29.8213557808E+00  ;
    const double A5 = 2.62433121679E+00  ;
    const double A6 = 48.6959930692E+00  ;
    const double A7 = 5.92885724438E+00  ;
    const double B0 = 0.398942280385E+00 ;
    const double B1 = 3.8052E-08	     ;
    const double B2 = 1.00000615302E+00  ;
    const double B3 = 3.98064794E-04     ;
    const double B4 = 1.98615381364E+00  ;
    const double B5 = 0.151679116635E+00 ;
    const double B6 = 5.29330324926E+00  ;
    const double B7 = 4.8385912808E+00   ;
    const double B8 = 15.1508972451E+00  ;
    const double B9 = 0.742380924027E+00 ;
    const double B10 = 30.789933034E+00  ;
    const double B11 = 3.99019417011E+00 ;
    double q;
    double y;

    //  |X| <= 1.28.
    if ( fabs ( x ) <= 1.28E+00 ) {

        y = 0.5E+00 * x * x;

        q = 0.5E+00 - fabs ( x ) * ( A1 - A2 * y / ( y + A3 - A4 /
                                                              ( y + A5 + A6 / ( y + A7 ) ) ) );
    }

        //  1.28 < |X| <= 12.7
    else if ( fabs ( x ) <= 12.7E+00 ) {

        y = 0.5E+00 * x * x;

        q = exp ( - y ) * B0 / ( fabs ( x ) - B1
                                 + B2 / ( fabs ( x ) + B3
                                          + B4 / ( fabs ( x ) - B5
                                                   + B6 / ( fabs ( x ) + B7
                                                            - B8 / ( fabs ( x ) + B9
                                                                     + B10 /( fabs ( x ) + B11 ) ) ) ) ) );
    }

        //  12.7 < |X|
    else {
        q = 0.0E+00;
    }


    //  Take account of negative X.
    if ( x < 0.0E+00 ) {
        cdf = q;
    }
    else {
        cdf = 1.0E+00 - q;
    }
}

/* Invert the Normal CDF.
 *
 * Licensing:
 *     This code is distributed under the GNU LGPL license.
 * Modified:
 *     - 23 February 1999
 *     - Extracted: November, 2016
 * Author:
 *     - John Burkardt
 *     - Extracted by Philipp Sommer
 *
 * Input
 *  cdf : the value of the CDF. 0.0 <= CDF <= 1.0.
 *  a   : the mean of the pdf
 *  b   : the standard deviation of the pdf
 *

/* Calculate the natural logarithm of GAMMA ( X ).
 *
 * Computation is based on an algorithm outlined in references 1 and 2.
 * The program uses rational functions that theoretically approximate
 * :math:`\log(\Gamma(X))` to at least 18 significant decimal digits.  The
 * approximation for 12 < X is from Hart et al, while approximations
 * for X < 12.0E+00 are similar to those in Cody and Hillstrom,
 * but are unpublished.
 *
 * The accuracy achieved depends on the arithmetic system, the compiler,
 * intrinsic functions, and proper selection of the machine dependent
 * constants.
 *
 *  Licensing:
 *    This code is distributed under the GNU LGPL license.
 *  Modified:
 *    - 16 June 1999
 *    - Extracted June, 2016
 *  Author:
 *    - Original FORTRAN77 version by William Cody, Laura Stoltz.
 *    - FORTRAN90 version by John Burkardt.
 *    - Extracted by Philipp Sommer
 *  Reference:
 *      - William Cody, Kenneth Hillstrom,
 *        Chebyshev Approximations for the Natural Logarithm of the Gamma Function,
 *        Mathematics of Computation,
 *        Volume 21, 1967, pages 198-203.
 *      - Kenneth Hillstrom,
 *        ANL/AMD Program ANLC366S, DGAMMA/DLGAMA,
 *        May 1969.
 *      - John Hart, Ward Cheney, Charles Lawson, Hans Maehly,
 *        Charles Mesztenyi, John Rice, Henry Thacher, Christoph Witzgall,
 *        Computer Approximations, Wiley, 1968.
 *
 *  Local Parameters:
 *
 *    Local, real ( kind = 8 ) BETA, the radix for the floating-point
 *    representation.
 *
 *    Local, integer MAXEXP, the smallest positive power of BETA that overflows.
 *
 *    Local, real ( kind = 8 ) XBIG, the largest argument for which
 *    LN(GAMMA(X)) is representable in the machine, the solution to the equation
 *      LN(GAMMA(XBIG)) = BETA**MAXEXP.
 *
 *    Local, real ( kind = 8 ) FRTBIG, a rough estimate of the fourth root
 *    of XBIG.
 *
 *  Approximate values for some important machines are:
 *
 *                            BETA      MAXEXP         XBIG     FRTBIG
 *
 *  CRAY-1        (S.P.)        2        8191       9.62E+2461  3.13E+615
 *  Cyber 180/855 (S.P.)        2        1070       1.72E+319   6.44E+79
 *  IEEE (IBM/XT) (S.P.)        2         128       4.08E+36    1.42E+9
 *  IEEE (IBM/XT) (D.P.)        2        1024       2.55E+305   2.25E+76
 *  IBM 3033      (D.P.)       16          63       4.29E+73    2.56E+18
 *  VAX D-Format  (D.P.)        2         127       2.05E+36    1.20E+9
 *  VAX G-Format  (D.P.)        2        1023       1.28E+305   1.89E+76
 *
 * Input
 *  x : the argument of the Gamma function (> 0.0)
 */
double gamma_log( double x ) {

    double c[7] = {
            -1.910444077728E-03,
            8.4171387781295E-04,
            -5.952379913043012E-04,
            7.93650793500350248E-04,
            -2.777777777777681622553E-03,
            8.333333333333333331554247E-02,
            5.7083835261E-03 };
    double corr;
    const double D1 = -5.772156649015328605195174E-01;
    const double D2 =  4.227843350984671393993777E-01;
    const double D4 =  1.791759469228055000094023E+00;

    const double FRTBIG = 1.42E+09;
    const double P1[8] = {
            4.945235359296727046734888E+00,
            2.018112620856775083915565E+02,
            2.290838373831346393026739E+03,
            1.131967205903380828685045E+04,
            2.855724635671635335736389E+04,
            3.848496228443793359990269E+04,
            2.637748787624195437963534E+04,
            7.225813979700288197698961E+03 };
    const double P2[8] = {
            4.974607845568932035012064E+00,
            5.424138599891070494101986E+02,
            1.550693864978364947665077E+04,
            1.847932904445632425417223E+05,
            1.088204769468828767498470E+06,
            3.338152967987029735917223E+06,
            5.106661678927352456275255E+06,
            3.074109054850539556250927E+06 };
    const double P4[8] = {
            1.474502166059939948905062E+04,
            2.426813369486704502836312E+06,
            1.214755574045093227939592E+08,
            2.663432449630976949898078E+09,
            2.940378956634553899906876E+10,
            1.702665737765398868392998E+11,
            4.926125793377430887588120E+11,
            5.606251856223951465078242E+11 };
    const double PNT68 = 0.6796875E+00;
    const double Q1[8] = {
            6.748212550303777196073036E+01,
            1.113332393857199323513008E+03,
            7.738757056935398733233834E+03,
            2.763987074403340708898585E+04,
            5.499310206226157329794414E+04,
            6.161122180066002127833352E+04,
            3.635127591501940507276287E+04,
            8.785536302431013170870835E+03 };
    const double Q2[8] = {
            1.830328399370592604055942E+02,
            7.765049321445005871323047E+03,
            1.331903827966074194402448E+05,
            1.136705821321969608938755E+06,
            5.267964117437946917577538E+06,
            1.346701454311101692290052E+07,
            1.782736530353274213975932E+07,
            9.533095591844353613395747E+06 };
    const double Q4[8] = {
            2.690530175870899333379843E+03,
            6.393885654300092398984238E+05,
            4.135599930241388052042842E+07,
            1.120872109616147941376570E+09,
            1.488613728678813811542398E+10,
            1.016803586272438228077304E+11,
            3.417476345507377132798597E+11,
            4.463158187419713286462081E+11 };
    double res;
    const double SQRTPI = 0.9189385332046727417803297E+00;
    const double XBIG   = 4.08E+36;
    double xden;
    double xm1;
    double xm2;
    double xm4;
    double xnum;
    double xsq;

    //  Return immediately if the argument is out of range.
    if ( x <= 0.0E+00 || XBIG < x ) {
        return DHUGE;
    }
    if ( x <= D_EPSILON ) {
        res = -log ( x );
    }
    else if ( x <= 1.5E+00 ) {

        if ( x < PNT68 ) {
            corr = - log ( x );
            xm1 = x;
        }
        else {
            corr = 0.0E+00;
            xm1 = ( x - 0.5E+00 ) - 0.5E+00;
        }

        if ( x <= 0.5E+00 || PNT68 <= x ) {

            xden = 1.0E+00;
            xnum = 0.0E+00;

            for(int i=0; i<8; i++) {
                xnum = xnum * xm1 + P1[i];
                xden = xden * xm1 + Q1[i];
            }
            res = corr + ( xm1 * ( D1 + xm1 * ( xnum / xden ) ) );
        }
        else {

            xm2 = ( x - 0.5E+00 ) - 0.5E+00;
            xden = 1.0E+00;
            xnum = 0.0E+00;
            for(int i=0; i<8; i++) {
                xnum = xnum * xm2 + P2[i];
                xden = xden * xm2 + Q2[i];
            }
            res = corr + xm2 * ( D2 + xm2 * ( xnum / xden ) );
        }
    }
    else if ( x <= 4.0E+00 ) {

        xm2 = x - 2.0E+00;
        xden = 1.0E+00;
        xnum = 0.0E+00;
        for(int i=0; i<8; i++) {
            xnum = xnum * xm2 + P2[i];
            xden = xden * xm2 + Q2[i];
        }
        res = xm2 * ( D2 + xm2 * ( xnum / xden ) );
    }
    else if ( x <= 12.0E+00 ) {

        xm4 = x - 4.0E+00;
        xden = - 1.0E+00;
        xnum = 0.0E+00;
        for(int i=0; i<8; i++) {
            xnum = xnum * xm4 + P4[i];
            xden = xden * xm4 + Q4[i];
        }
        res = D4 + xm4 * ( xnum / xden );
    }
    else {

        res = 0.0E+00;

        if ( x <= FRTBIG ) {

            res = c[6];
            xsq = x * x;
            for(int i=0; i<6; i++) {
                res = res / xsq + c[i];
            }
        }
        res = res / x;
        corr = log ( x );
        res = res + SQRTPI - 0.5E+00 * corr;
        res = res + x * ( corr - 1.0E+00 );
    }
    return res;
}
/* Compute the incomplete Gamma function.
 *
 * Formulas:
 *     .. math::
 *         \Gamma_{inc}(P, 0) = 0
 *
 *     .. math::
 *         \Gamma_{inc}(P, \infty) = 1.
 *
 *     .. math::
 *         \Gamma_{inc}(P,X) = \int_0^x{T^{P-1} \exp{(-T)} \mathrm{d}t} / \Gamma(P)
 *
 * Licensing:
 *     This code is distributed under the GNU LGPL license.
 * Modified:
 *     - 01 May 2001
 *     - Extracted: June, 2016
 * Author:
 *     - Original FORTRAN77 version by B L Shea.
 *     - FORTRAN90 version by John Burkardt
 *     - Extracted by Philipp Sommer
 * Reference:
 *    BL Shea,
 *    Chi-squared and Incomplete Gamma Integral,
 *    Algorithm AS239,
 *    Applied Statistics,
 *    Volume 37, Number 3, 1988, pages 466-473.
 *
 * Input
 *  p  : the exponent parameter (0.0 < P)
 *  x  : the integral limit parameter. If X is less than or equal to 0, GAMMA_INC is returned as 0.
 */
double gamma_inc ( double p, double x ) {

    double  a;
    double  arg;
    double  b;
    double  c;
    double  cdf = 0.;
    const double EXP_ARG_MIN = -88.0E+00;
    const double L_OVERFLOW = 1.0E+37;
    const double PLIMIT = 1000.0E+00;
    double  pn1;
    double  pn2;
    double  pn3;
    double  pn4;
    double  pn5;
    double  pn6;
    double  rn;
    const double TOL = 1.0E-07;
    const double XBIG = 1.0E+08;

    double gamma_inc = 0.0;

    if ( p <= 0.0E+00 ) {
        printf("\n GAMMA_INC - Fatal error! \n");
        printf("  Parameter P <= 0.");
        exit(-1);
    }

    if ( x <= 0.0E+00 ) {
        return 0.0E+00;
    }

    //  Use a normal approximation if PLIMIT < P.
    if ( PLIMIT < p ) {
        pn1 = 3.0E+00 * sqrt ( p ) * ( pow(( x / p ), ( 1.0E+00 / 3.0E+00 ))
                                       + 1.0E+00 / ( 9.0E+00 * p ) - 1.0E+00 );
        normal_01_cdf ( pn1, cdf );
        return cdf;
    }

    //  Is X extremely large compared to P?
    if ( XBIG < x ) {
        return 1.0E+00;
    }
    //  Use Pearson's series expansion.
    //  (P is not large enough to force overflow in the log of Gamma.
    if ( x <= 1.0E+00 || x < p ) {

        arg = p * log ( x ) - x - gamma_log( p + 1.0E+00 );
        c = 1.0E+00;
        gamma_inc = 1.0E+00;
        a = p;

        for (int j=0; j<1; j+=0) {
            a = a + 1.0E+00;
            c = c * x / a;
            gamma_inc = gamma_inc + c;
            if ( c <= TOL ) {
                break;
            }
        }

        arg = arg + log ( gamma_inc );

        if ( EXP_ARG_MIN <= arg ) {
            gamma_inc = exp ( arg );
        }
        else {
            gamma_inc = 0.0E+00;
        }
    }
    else {
        //  Use a continued fraction expansion.
        arg = p * log ( x ) - x - gamma_log ( p );
        a = 1.0E+00 - p;
        b = a + x + 1.0E+00;
        c = 0.0E+00;
        pn1 = 1.0E+00;
        pn2 = x;
        pn3 = x + 1.0E+00;
        pn4 = x * b;
        gamma_inc = pn3 / pn4;

        for (int j=0; j<1; j+=0) {

            a = a + 1.0E+00;
            b = b + 2.0E+00;
            c = c + 1.0E+00;
            pn5 = b * pn3 - a * c * pn1;
            pn6 = b * pn4 - a * c * pn2;

            if ( 0.0E+00 < fabs ( pn6 ) ) {

                rn = pn5 / pn6;

                if ( fabs ( gamma_inc - rn ) <= std::min( TOL, TOL * rn ) ) {

                    arg = arg + log ( gamma_inc );

                    if ( EXP_ARG_MIN <= arg ) {
                        gamma_inc = 1.0E+00 - exp ( arg );
                    }
                    else {
                        gamma_inc = 1.0E+00;
                    }
                    return gamma_inc;
                }
                gamma_inc = rn;
            }

            pn1 = pn3;
            pn2 = pn4;
            pn3 = pn5;
            pn4 = pn6;

            //  Rescale terms in continued fraction if terms are large.
            if ( L_OVERFLOW <= fabs ( pn5 ) ) {
                pn1 = pn1 / L_OVERFLOW;
                pn2 = pn2 / L_OVERFLOW;
                pn3 = pn3 / L_OVERFLOW;
                pn4 = pn4 / L_OVERFLOW;
            }
        }
    }
    return gamma_inc;
}
