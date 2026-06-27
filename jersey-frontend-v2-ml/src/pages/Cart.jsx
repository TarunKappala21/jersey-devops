import { useEffect, useState } from 'react'
import { useNavigate } from 'react-router-dom'
import { getCart, removeFromCart, checkout } from '../api/client'
import { useAuth } from '../context/AuthContext'
import toast from 'react-hot-toast'
import { Trash2, ShoppingBag, ArrowRight } from 'lucide-react'

export default function Cart() {
  const { user }              = useAuth()
  const navigate              = useNavigate()
  const [cart, setCart]       = useState(null)
  const [loading, setLoading] = useState(true)
  const [checking, setChecking] = useState(false)

  const fanId = user?.fanId || 101

  const fetchCart = () => {
    getCart(fanId)
      .then(r => setCart(r.data.data))
      .catch(() => {})
      .finally(() => setLoading(false))
  }

  useEffect(() => { fetchCart() }, [])

  const handleRemove = async jerseyId => {
    await removeFromCart(fanId, jerseyId)
    toast.success('Item removed')
    fetchCart()
  }

  const handleCheckout = async () => {
    setChecking(true)
    try {
      const res = await checkout(fanId)
      const d   = res.data.data
      toast.success(`Order #${d.orderId} placed! Total: ₹${d.grandTotal?.toFixed(0)}`)
      navigate('/')
    } catch {
      toast.error('Checkout failed')
    } finally {
      setChecking(false)
    }
  }

  if (loading) return <div style={{ textAlign: 'center', padding: 60, color: 'var(--text-muted)' }}>Loading cart...</div>

  const isEmpty = !cart?.items?.length

  return (
    <div className="container" style={{ padding: '40px 24px', maxWidth: 800 }}>
      <h2 style={{ marginBottom: 32 }}>YOUR <span className="accent">CART</span></h2>

      {isEmpty ? (
        <div className="empty-state">
          <ShoppingBag size={48} />
          <p>Your cart is empty</p>
          <button className="btn btn-primary" onClick={() => navigate('/jerseys')}>
            Browse Jerseys
          </button>
        </div>
      ) : (
        <div style={{ display: 'flex', flexDirection: 'column', gap: 16 }}>

          {/* Items */}
          {cart.items.map(item => (
            <div key={item.jerseyId} className="card" style={{
              padding: '20px', display: 'flex',
              justifyContent: 'space-between', alignItems: 'center', gap: 16,
            }}>
              <div style={{ display: 'flex', alignItems: 'center', gap: 16, flex: 1 }}>
                <span style={{ fontSize: '2rem' }}>
                  {item.sport === 'Football' ? '⚽' : '🏏'}
                </span>
                <div>
                  <p style={{ fontWeight: 600, marginBottom: 2 }}>{item.jerseyName}</p>
                  <p style={{ fontSize: '0.8rem', color: 'var(--text-secondary)' }}>
                    {item.team} · {item.player}
                  </p>
                  {item.discount > 0 && (
                    <span className="badge badge-limited" style={{ marginTop: 4 }}>
                      {item.discount}% off
                    </span>
                  )}
                </div>
              </div>

              <div style={{ textAlign: 'right' }}>
                <p style={{ fontFamily: 'var(--font-display)', fontSize: '1.3rem', color: 'var(--accent)' }}>
                  ₹{item.subtotal?.toFixed(0)}
                </p>
                <p style={{ fontSize: '0.75rem', color: 'var(--text-muted)' }}>
                  ₹{item.unitPrice?.toFixed(0)} × {item.quantity}
                </p>
              </div>

              <button className="btn btn-danger"
                onClick={() => handleRemove(item.jerseyId)}
                style={{ padding: '8px' }}>
                <Trash2 size={15} />
              </button>
            </div>
          ))}

          {/* Summary */}
          <div className="card" style={{ padding: '24px', marginTop: 8 }}>
            {[
              { label: 'Subtotal',    value: cart.subtotal },
              { label: 'GST (18%)',   value: cart.gst },
            ].map(row => (
              <div key={row.label} style={{
                display: 'flex', justifyContent: 'space-between',
                marginBottom: 10, color: 'var(--text-secondary)', fontSize: '0.9rem',
              }}>
                <span>{row.label}</span>
                <span>₹{row.value?.toFixed(2)}</span>
              </div>
            ))}
            <hr className="divider" />
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
              <span style={{ fontFamily: 'var(--font-display)', fontSize: '1.4rem' }}>GRAND TOTAL</span>
              <span style={{ fontFamily: 'var(--font-display)', fontSize: '1.8rem', color: 'var(--accent)' }}>
                ₹{cart.grandTotal?.toFixed(0)}
              </span>
            </div>

            <button className="btn btn-primary" onClick={handleCheckout} disabled={checking}
              style={{ width: '100%', justifyContent: 'center', marginTop: 20, padding: '14px', fontSize: '1rem' }}>
              <ArrowRight size={16} />
              {checking ? 'Placing order...' : 'Confirm Order'}
            </button>
          </div>
        </div>
      )}
    </div>
  )
}
