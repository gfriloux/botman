#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>
#include <Gotham.h>

int alfred_log_dom;

#define AVATAR "iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABmJLR0QA/wD/AP+gvaeTAAAACXBI"\
               "WXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH3AgKDCcUP+BCLQAAE8NJREFUeNrlm3l4W+WVxn/fvVeS"\
               "JVmWZUm25X1f4myQjRACISEQkpYlBMpTYDKUDi1dhq0t27SlpQsMpdApgbRM+0AI0FJKCAEKadIE"\
               "SMjubGRxnMWJ93i3bMla7v3mDztyjBNnbZI+o//0XEn3+977nve855xP8P/8Jc7x/UqunjR22iWj"\
               "Ske6nI7MeJs1CZAAoXAk2NDcemDDtt3lH63euAyo+JcHwG6NozvYw6SLhz8w94ar/2vG5PEunzcJ"\
               "XdelrhtCNwyEEEgpUYRA1VSpqaqob2rlw0/XN774xruPb961d/6/MgNKX/rpA6vvnD3DFQyFT/W7"\
               "0mIyifl/XlL7nz9/fhJw8J+xQPWftXO3M+HKVa8/t37KhIviIlH9dIAWumEwbniRozQr9/5lazd9"\
               "GopEDvyrAJC/YsEzm0pys+SZskw3DOFOdMqSjNy5S9duKI/q+p6zuVDln7H73zz2nU9HFOZKIcQZ"\
               "h5gQgkSnXfg8bjn/0e+9a9K0yRc0ACOL8779ja98yWdIedb0JS7OhJRSpLqT5NP3fusTIPGCBeD+"\
               "uXN+GYlE5VmPVU1FVVUxsjBf3j7z6pcvSADys9LvvOP66Y4DNfVCyrOLgUCy/vOdCCHE1HFjrge8"\
               "FxwAD/z7TQ8ZuiF3769GUc4uuQwDpJQYhkFeehrTxo35zoUGQPrUCRcVhyMRoSgCk6adVQAUIQCB"\
               "ISXhSISLS4vmXFAAXDf10jsKstORgKIIxFm0WLre6xj7XDMAPo972AUFwIzJ4+b0hMJYzCaCPWHC"\
               "kehgIVMUVq7fwmebdxCORLGYTVjMJuIsZlraO1m7ZSdNbR3HoL8kqusEw2HMfcwqyckGGHWm6z5r"\
               "PM3PTBsjpeRATQM56akYxpGn1v+K6jrXXDaOzq5uNmzfTXewB0UIdMMgLdnDiOI8HHYbhmF8gQE6"\
               "AKMKC9i0q4LhBXnYLBbSvO7iuqaWrRcCAFMmjxmBlJI1W3dy26xp9ITDaKpKJBqNASGEIBSOYDGb"\
               "mXTx8OOInTHICJlUjYqqQwwvzMUWZ+ktFIDS3JyMuqaW88+A27981URNU9F1g5FFeRhS8tyCv7Lx"\
               "8z3c92+zmTx2JIFgD6dqDHXD4CfPL+C5BX/lSFq9ZfqV3HndLMxCSGe8PeGCCIHS/KxiXTdQVYWa"\
               "hiauuON+/N0BABYvX83Fwwp56YkHKSvIIRyJDAmElBK7LY4XXn+X7/7st4Ouv/n3FSz55DMe+9od"\
               "wjBk9IIAoN3f3RGJRuWN331CfPjp+kHXy3dWMuamb3LT1Zcz//H7sFrMHM8mdQd7uHLuA2zaUYkQ"\
               "gokjhzO6qICirEyyUlPwunpdsKoq/OaNtyoviH6Ax5Vw48oFz77d2NIW+8FAsIcOfxebtu9h864q"\
               "qmprOVDfCMAv7r+Lh75+K919YSGlxGwy8d7KNcy59ye47DamT7qEYTnZGFJiUlVc8XYKc7LRVBXd"\
               "MKhpPMzXfvpkEtB2vtOgd/KYkbMCPSHGjyihMDuD9XsbkL5h7AvZ+aTNw4qN5SiZF/PxyhU8+thj"\
               "7Kqo5rFnXkbvEzwhBH9fvYmNm/cx75mnae3qxp99JU+/+R4f+92UzJjNXT9/mj8sXUlVXT0A+2rq"\
               "dlgt5tFup/OW89YPcDkd13700lNbfvDN2y76dN1WUZSbgc1qIRoKkj92CrNvuIEDVdX4ii/hP26d"\
               "yXVfmsGaD99n1oQxtLZ3MmncMKSUSClJciRg06wcrqkmfdhIrp1YSuHoy5k8IoObb/wyry14lRHO"\
               "OFLcbiwmE0vXrXf98jvf/Npt106/Od5m/db6HbteACLnDIDstJRrP1n47AeF2elSj0TFnoM1lBXm"\
               "oOsG2Wkp7N64ivVbd1JalMslI/OQXW3c9617mDl2FGXDsogQoiQvK+byVFVQU9tKRrKX/37qSdwu"\
               "B0nxGkq4iw3LFnPVMB8d7SEURaGzO0BifLzqcTmRUjKiIM8WikRyP9+3/+1zJoI/vOf2Z1VFkQ3N"\
               "rSLN6+4tfvpSlWEYjCkrIs5sRlHg49XlbFi3hXmP3oMv1YWUvfb26IrRYjEBEI5E+Oq1V7N3xxZm"\
               "TRlHXmEagcIkFEVgs5np6YmwtXIvUy4eDUKwass2Lhk+TGSmJOeeUw2obmjampWWwuIP1rFo2Wpm"\
               "Xj6BZWvKBxiYUCTC39eU09TZwY/uu41krxPDkKiKgqooRPWBpsdkUnuNj6Yxa/JEWju7qKiqicl1"\
               "T08ERVEQCEwmjb8uX8n4slJUVWF/bd3acwrAT+YteKGzKyCmXnoRTpOT1xYvJxyJDkorl44uY9bU"\
               "8YTD/Y5QUQSyL+cPTEn9jrGtvYuygmwyU3vL/kB3GMOQaKqKpir8eek/uO6Ky5BSUt/cwlvLVz5/"\
               "rrPAxy8v+nDvsJIMeeBwDUV5GaxctwWLxTzwBn19gYHeR8RMT3+dYBAK92tYIBCKASalpLMzgKL0"\
               "VoRrP99JmtfDwfoGNFWVH63ZsALYf87T4INPzb+zua1DlBZmkZ7iYc6MK2hsPnFaNoze+Ff7wJFS"\
               "0tLiR1X7lxMMhmMuRQhBpz+IEIKq+kbK8nKYMLyUhpZWDtTVi1fe+9vc8+UDVj3x4qtvTZ80hq27"\
               "95HosPPJxm2cqB0W1fVeOmtqLCSamjoHWeRwKBq77vcHkVJS3dDIzEkTWbt9JxPKSpn35ts/A6rP"\
               "mxH6/Zvv3/zSm++3Xzd1oizKyURRlCG7QZs+30NHVzcWs4lwOBqr99vautm+dz/7ampjURLqAwAB"\
               "3d0hFEWgKAr+QJAEu43fL1qydnNF5Q/PqxOMs5i5+0e/Lpz/pyUtoXCYTTv2YDGbjvlZk6bR7u9C"\
               "SsnUSy5i6ZqNADQ2dqAoguEFebT5/TGVMPqYdIQJmqrxynt/48mXF/Jx+Zblf1m2YuJ5L4Z6QmGs"\
               "FnPzvb+Y5733F/NGAy0zL59waOzwomPGfmNLG3ZrHGaTRk9PGJOmUt/QhtlkYktFJRaTGSkljngr"\
               "CY44pIRIRI8BWHmohspDNQCPnI065qxUg0cNPg9k5xa2fFq+U44dXiSOFft3zp7BkhVr6OwK4HTY"\
               "QSrs2neQfbX1TCgrJd5qxedzkZebEmuOqErvT9U1NceqZuCXwFUXUlfY4/GmbHI6E5XlG3YIuzVu"\
               "cOnZ1xGaMXkcU8aN4OFnX+aqux/jmb8soSccJtWbyNix+eTmJGMYEiF6Y762vg1VUdh1oOroPDoN"\
               "WAYknbdi6MgrPt7xgKJqyzKz8lyAqGlsJjc1kaLsjAF1v5QSk6byqwXv8sKH28guKMXfHSLB5SZs"\
               "c/Fh+U6qa2spy/ERb7PGvtvZEeCzLTvZvnc/2/cOSPe5wENACFh1PgCYmp6Rs9qZmHSTxWLBZrML"\
               "IQQ2q41gVzuzrrgEo6/ai7OYWbl+Kw/Mf5ewzYfVakNRTbi8ySS4PJgtFhKcLgLCzsKl62isr+Hy"\
               "MWVEozqhUITyHZWs3b6D2v4wOLqfcRUwF1gEdJyLhog3OSXtLa835XJV02TV/kqRk1eElL0xa0jJ"\
               "9786ndlXTYo9+W8/8VsSPakUZiTjtFtwWM3Ex5mRSASCzkAIf0+E9q4gbV0hmvw9bNtazss/e4B4"\
               "q40tWw4w897vD7WmI6P4bwMvnHURVBQFwzCIdyT8ONWX8bjVapcgqa0+KDKycjEMPWZkQuEIRVk+"\
               "ANo6u1j8j9W8+tTDBEMholH9hM1RRVHQVAWz6TZeW7Kc3IxUrBYLya5EDre1D/UwJTAPmA1MHzBJ"\
               "OVMADMMYk56Z84Hb7U3utbKGCIdDaCYNVR0YSV3dXRTnZWE2mfjg43V845Yv0xUIxjZnNmkDVh3V"\
               "dSJHAWMYBmHDIByJcv20SzFpGp+u2kVOmm8oAI5m9DSgC7gc2HTaAJjNZsLhMIku98upvoy5JpNJ"\
               "Stmvzg11NeTkFyG/0MtXBSQlJuDvCjD3hqsJR/qLnM6ubj7bvANVUdANAyl7ATBpGtdPu5RgT+iY"\
               "AxXdMEhOcp1KmFqBjX3p8tHTAiAcDo/Pys5blujyOAxDjyEspaS1tRm3JxlDH0xpT2I8Rl+nR/8C"\
               "OA67jWsuG/eFzKBhGAaLl/f2Fb74nUgkCn1Cehr69ghwA3AZ0HrSPiAxMel3JcNGrnMmJsVLOXDM"\
               "pagqHe1tOBKcx4zn/mHmSaxSiN7iSEquuWwc0b4x2NEA+f1BzGYTew/VnG62KgFagK+cEABV1eZk"\
               "5eQHs3Ly79Y00zEzRUtTI6mp6YPGWLHrnQFa2js5nUMSRwMnpURVFQ4eaqauqYl6f+BM2v+yuGTE"\
               "n1xuzyIAra9gOxqAktS0zK2lZaP+4nS6LEdi/VgK3dXlJ85qPe5T9rjdPPzsH7HFWTiTkyKKIti5"\
               "q4bDze08Mv+PFBYUnvZvFRUPFyazmczM3Bsys/M6otHoqCNGyOHxpryalZ3/oiPBmdx3skscb2zl"\
               "7+zAarVhMpuPC4Cu60yaNot33v+AKeNGYNY0dMM44UisV1wFoVCUpqZONm3Zx6tLPuKR539Hp99P"\
               "qi+N2tpTD4O8/GKsNnvs/nFxVrMryXNPONwjtLyCks74eMcROosT5ee2thZycguOS/9e0YqQkuJl"\
               "yeJ9JI67jhuvuoyZV0ygND+Lwux0PInOWKl7pP3V0NRGxf4aPq+o6q0SzSZWbihn+Yb+LNbedupD"\
               "oMzsPOzxjkEhYTKZyM7O/7Fmt8dzPLp/8QmFQiFsNvuQmz9iyqQEi6V3lL1o2SoWL/+MW6ZfydiS"\
               "YkLRKELptSm6YSANid1mxedJ4oqLRlNdX8976zcP2LzJZCYUDp3S5jMys0lMTDq+1ghx8tWgEIKm"\
               "w/V4k1NP+NloNILVaqWgML/fSEmDsslTGHXdbKTTxYHaeiK6jjPejiPeRrvfz/ur1vLcO+8zce7d"\
               "ZAwbeH4gv6CApqbDxzc0mgm3J5mMzFwyMnMASHInnz0rbBgGmqadlKipqgm/388VU6aw4JWFsRqh"\
               "oqKC2tpa5s8f+gD4+m2XDXKXebn57N61c3C6drnxJqfGmCmlRFEUmpsP09zUiNuTfHYAaKivwZeW"\
               "edLqHY1EmDbtGuIdifg7e/3HwoULY5+x2e1Y46y0tPRWd6mpPoLBIB0d7Rw8OPhguCvJPeC90+ki"\
               "PTMHVVVjx+eOMNUwDFJ9GVQf3I83OXXIkNVONo0aJ6ET/XQ009nZiaaZGT9+LJWV+zh0cF/s+o03"\
               "3szcO+/CYrGwt7ISVVUpKCwkGOzhT2+8yhuvvzroN4uLS/qbAPnFOBwJAzb9xXB1OhM5hMTv78R2"\
               "VAY4LQAON9aRkuI7KbHsdYoKLS0tNLe08NAjD/Lg/Q8zZuwExowZy4xrv4TH6yIpyUlPMITPdyle"\
               "rwtXkpMuf4CMjBTm3Hwrry18hcWL30aPRplz860Egr0mqLRsNJqmnnAthmHgS8vkcEMdeQXFxw1d"\
               "7cR0Vgj1BDGbT97UKELQ0NBATzDExIkT2bp1A7v37Ke1pY3MrDTc7sRBtIxGdSxxZgqLcigszCE9"\
               "I5W7vv4NItEoihAsWvQWpWWj+rShd+PdXX4kkJDgPCbN3W4vtdVVRKMRVFU7dQCOFD1JnuRjHns7"\
               "inMofbF35Hu1dbWEQhGEgK5ANxkZKWRm9sbj8WIyNgpDUlZWSGdnFw31TTidDpYujaPPmve35K02"\
               "dn6+GUVRcLuTcXuTMZstsbVKKUlOTeNwY/1x9Us50dPv7GjH4UgYsHkhBEIIAt3dNB1uoGrfHg7s"\
               "rxzAkJrq6j4AlEFu72RSrpSS+HgbhUU5ZGWnsadi7zHXVzJsFIZh0NTUwO6d29hTsYP2tv6jc8nJ"\
               "PpqbGk9dA6SUhEMhbPbe9KIoCj09QdraWvF3tBEM9hcmzsQksnMLMI6q5ISiUFV1gHHjR/QNNU+j"\
               "gulbtKZo7Nq1m0RX0qDrmqZRWFxGZcWO3jlFMEBNdRU11VV9KdJHkttLW2szzsSkQUBoQ928ubmR"\
               "VF8GjQ21tDQ3oeuDT6W5PcmkZ2QP6gs4HAmUb9rItTOnkZAQf0ad1527d6EcJ4aFEMTFWcnNL+LA"\
               "voH/pmlva6G9rQVFVQn1BHEleQbpmDIU/TvaW9mxvZzDjfXoehSzxYIryU2qLwNXkie2+WNRWwjB"\
               "oncWU32o+owqQlVVeWfROzgcCSdozSeQmZUbW3uSu//vBIauo2qmY65DTUlNe/x4IYAAh8NJSkoa"\
               "GZk5eL0+nE4XkWgEaRikpWcNKY6aqrGxfBu3f/XmoUV0iIfwwQd/46mn/weL2XLiHpjNjpTQ1dWJ"\
               "UBRKh49G1TRCPT0EAl340jJPngEAXm8qHm8KNnt8HygGrS1NRCNhUn0ZJxS1UDiIw5nMg997BKvV"\
               "esrxv+IfK1jwxhI0RYnZ6RNlrVRfBokuN4HuLg7srcDjSaG0bBT5BSVEo5EzGY0JWpobAYHbk3JS"\
               "tLbb4qmvq6bVH+V/X/rDKf2LZP36Dcz7/evYrDZ0XT9p9hiGTlZ2PvZ4B35/B4cO7kVKic0ef8z7"\
               "n/SKWpobUFRtkCcf0hKbTHT7O9A0jfc+XMWGDRtOCriKigqefPoFPN5k6msPET+4nh+SOYahk5df"\
               "3CeErdTVHjqz4Wgg0I1hSFwu9ynRWEpJqCeAHjVI8nh48ukXqaurGxKEit0V/PDxX5HiS0cIgb/j"\
               "1JsgiqJQU111FCjKmQFgs9kJBrpPWc2llEQiPQQDvZ4hOTWNB37wODU1x25rrVmzlh/99Nd4+noO"\
               "qqrS3tpyyvf1+ztpa22OMSLVl37cz/4fOnB2GrQcvw0AAAAASUVORK5CYII="

#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define VARGET(_a) gotham_citizen_var_get(citizen, _a)

#define CRI(...) EINA_LOG_DOM_CRIT(alfred_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(alfred_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(alfred_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(alfred_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(alfred_log_dom, __VA_ARGS__)

void alfred_emoticons_load(Gotham *gotham);
Eina_Bool alfred_event_connect(void *data, int type, void *ev);
Eina_Bool alfred_event_citizen_disconnect(void *data, int type, void *ev);
Eina_Bool alfred_event_citizen_command(void *data, int type, void *ev);
Eina_Bool alfred_event_citizen_list(void *data, int type, void *ev);

char *alfred_utils_elapsed_time(double timestamp);
